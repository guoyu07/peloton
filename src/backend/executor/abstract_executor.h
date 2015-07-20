/**
 * @brief Header file for abstract executor.
 *
 * Copyright(c) 2015, CMU
 */

#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "backend/concurrency/transaction_manager.h"
#include "backend/executor/executor_context.h"
#include "backend/executor/logical_tile.h"
#include "backend/planner/abstract_plan_node.h"

namespace peloton {
namespace executor {

class AbstractExecutor {

 public:
  AbstractExecutor(const AbstractExecutor &) = delete;
  AbstractExecutor& operator=(const AbstractExecutor &) = delete;
  AbstractExecutor(AbstractExecutor &&) = delete;
  AbstractExecutor& operator=(AbstractExecutor &&) = delete;

  virtual ~AbstractExecutor() {
  }

  bool Init();

  bool Execute();

  //===--------------------------------------------------------------------===//
  // Children + Parent Helpers
  //===--------------------------------------------------------------------===//

  void AddChild(AbstractExecutor *child);

  const std::vector<AbstractExecutor*>& GetChildren() const;

  //===--------------------------------------------------------------------===//
  // Accessors
  //===--------------------------------------------------------------------===//

  // Virtual because we want to be able to intercept via the mock executor
  // in test cases.
  virtual LogicalTile *GetOutput();

  const planner::AbstractPlanNode *GetRawNode() const {
    return node_;
  }

 protected:

  explicit AbstractExecutor(planner::AbstractPlanNode *node,
                            ExecutorContext *executor_context = nullptr);

  /** @brief Init function to be overriden by derived class. */
  virtual bool DInit() = 0;

  /** @brief Workhorse function to be overriden by derived class. */
  virtual bool DExecute() = 0;

  void SetOutput(LogicalTile* val);

  /**
   * @brief Convenience method to return plan node corresponding to this
   *        executor, appropriately type-casted.
   *
   * @return Reference to plan node.
   */
  template<class T> inline const T& GetPlanNode() {
    const T *node = dynamic_cast<const T *>(node_);
    assert(node);
    return *node;
  }

  /** @brief Children nodes of this executor in the executor tree. */
  std::vector<AbstractExecutor*> children_;

 private:

  // Output logical tile
  // This is where we will write the results of the plan node's execution
  std::unique_ptr<LogicalTile> output;

  /** @brief Plan node corresponding to this executor. */
  const planner::AbstractPlanNode *node_ = nullptr;

 protected:

  // Executor context
  ExecutorContext *executor_context_ = nullptr;

};

}  // namespace executor
}  // namespace peloton
