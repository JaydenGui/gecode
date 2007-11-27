/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2006
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef GECODE_GIST_SPACENODE_HH
#define GECODE_GIST_SPACENODE_HH

#include "gecode/gist/node.hh"
#include "gecode/gist/better.hh"
#include "gecode/kernel.hh"
#include "gecode/int.hh"

namespace Gecode { namespace Gist {

  /// \brief Status of nodes in the search tree
  enum NodeStatus {
    SOLVED,       ///< Node representing a solution
    FAILED,       ///< Node representing failure
    BRANCH,       ///< Node representing a branch
    UNDETERMINED, ///< Node that has not been explored yet
    SPECIAL       ///< Node representing user controlled exploration
  };

  // TODO nikopp: put this somewhere else (e.g. to the same file as BranchingDesc)
  class SpecialDesc {
  public:
    const std::string vn;
    const IntRelType r;
    const int v;
    SpecialDesc(std::string varName, IntRelType r0, int v0);
  };
  
  /// \brief Static reference to the currently best space
  class BestSpace {
  public:
    /// The currently best space found, or NULL
    Space* s;
    /// The object used for constraining spaces to be better
    Better* b;
    /// Constructor
    BestSpace(Space* s0, Better* b);
  };

  /// \brief A node of a search tree of Gecode spaces
  class SpaceNode : public Node {
  private:
    /// A copy used for recomputation, or NULL
    Space* copy;
    /// Working space used for computing the status
    Space* workingSpace;
    
    union {
      /// Branching description
      const BranchingDesc* branch;
      /// Special branching description
      const SpecialDesc* special;
    } desc;
    
    /// Current status of the node
    NodeStatus status;
    
  protected:
    /// Reference to currently best space (for branch-and-bound)
    BestSpace* curBest;
  private:
    /// Reference to best space when the node was created
    Space*     ownBest;
    
    /// Number of children that are not fully explored
    int noOfOpenChildren;
    /// Whether the subtree of this node is known to contain failure
    bool _hasFailedChildren;
    /// Whether the subtree of this node is known to contain solutions
    bool _hasSolvedChildren;
    
    /// Recompute workingSpace from a copy higher up, return distance to copy
    int recompute(void);
    /// Try to get workingSpace from parent
    Space* donateSpace(int alt, Space* ownBest);
    /// Try to get copy from parent if this node is the last alternative
    Space* checkLAO(int alt, Space* ownBest);
    /// Acquire working space, either through donateSpace or recompute
    void acquireSpace(void);

    /// Mark all ancestors as having solved children
    void solveUp(void);
    /// Book-keeping of open children
    void closeChild(bool hadFailures, bool hadSolutions);
  public:
    /// Construct node for alternative \a alt
    SpaceNode(int alt, BestSpace* cb = NULL);
    /// Construct root node from Space \a root and branch-and-bound object \a better
    SpaceNode(Space* root, Better* b);
    /// Destructor
    virtual ~SpaceNode(void);

    /// Return working space.  Receiver must delete the space.
    Space* getSpace(void);
    
    /** \brief Compute and return the number of children
      *
      * On a node whose status is already determined, this function
      * just returns the number of children.  On an undetermined node,
      * it first acquires a Space (possibly through recomputation), and
      * then asks for its status.  If the space is solved or failed, the
      * node's status will be set accordingly, and 0 will be returned.
      * Otherwise, the status is SS_BRANCH, and as many new children will
      * be created as the branch has alternatives, and the number returned.
      */
    int getNumberOfChildNodes(void);
    
    /// Return current status of the node
    NodeStatus getStatus(void);
    /// Changes the NodeStatus to \a s
    void setStatus(NodeStatus s);
    /// Changes the SpecialDesc to \a d
    void setSpecialDesc(const SpecialDesc* d);
    
    /// Return alternative number of this node
    int getAlternative(void);
    /// Return whether this node still has open children
    bool isOpen(void);
    /// Opens all nodes on the path up to the root
    void openUp(void);
    /// Return whether the subtree of this node has any failed children
    bool hasFailedChildren(void);
    /// Return whether the subtree of this node has any solved children
    bool hasSolvedChildren(void);
    /// Return number of open children
    int getNoOfOpenChildren(void);
    /// Set number of open children to \a n
    void setNoOfOpenChildren(int n);
    /// Return whether the node has a copy
    bool hasCopy(void);
    /// Return whether the node has a working space
    bool hasWorkingSpace(void);
       
    /// Pseudo-constructor to allow creation of nodes of sub-classes from getNoOfChildNodes
    virtual SpaceNode* createChild(int alternative);
    /// Called when the status has changed
    virtual void changedStatus(void);
  };

}}

#endif

// STATISTICS: gist-any
