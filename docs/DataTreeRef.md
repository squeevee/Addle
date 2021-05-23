The DataTree utility subdirectory provides reusable code for different
sitautions modeling tree-shaped (a.k.a. heirarchical) data structures.

# API

The same generic API is used by all data tree utilities. Custom trees can
implement this API to use the same utilities.

The API is generally in the process of evolving and being refined.

The minimum requirements for use of the API are to create a valid NodeHandle 
type, provide a `datatree_traits` specialization with `node_handle` and 
`const_node_handle` member types, then overload the following free functions:
- `datatree_root`
- `datatree_node_parent`
- `datatree_node_children_begin`
- `datatree_node_children_end`
- `datatree_node_sibling_next`

However, it can be a good idea to overload more of the free functions if the
particular data tree implementation allows that information to be retrieved 
efficiently.

## datatree_traits

The class template `aux_datatree::datatree_traits` can be specialized for a 
tree class with the following member types as aliases where appropriate:

- `node_handle` giving the NodeHandle type for the tree (required)
- `const_node_handle` giving the NodeHandle type for const nodes in this tree
(required)

## NodeHandle

A NodeHandle must meet the following requirements:

- Default constructible as a null NodeHandle
- Copy constructible
- (Of NodeHandles referring to mutable nodes) Implicitly convertible to an 
otherwise equivalent NodeHandle referring to the same node as const.
- Copy assignable
- `==` and `!=` Comparable to other NodeHandles of the same tree type, including
NodeHandles of the same tree type referring to nodes of different constness.
- Contextually convertible to `bool`, where true indicates it refers to a valid
node and false indicates it is null
- Overloads `operator!` to give the inverse of conversion to `bool`
- Dereferenceable to an unspecified type

It is generally good practice for NodeHandle to be cheap to construct, copy, and
store. However, a notable exception to this practice is 
`aux_datatree::DFSStateHelper<..., false>`, which itself stores a potentially 
non-trivial array of ChildNodeHandles. It is therefore also good practice to
minimize copying and moving of NodeHandle, and to perform operations on it
in-place.

The value acquired by dereferencing a NodeHandle is not used directly by the 
API, but is occasionally exposed to users of data tree utilities as a "friendly"
representation of the referent node.

## ChildNodeHandle

A ChildNodeHandle is the type returned by `datatree_node_children_begin`. 

It must meet all the requirements of the corresponding NodeHandle and must be 
implicitly convertible *to* the corresponding NodeHandle. 

There is no requirement that ChildNodeHandle must be implicitly convertible from 
NodeHandle, and there is no requirement that any API functions that operate on 
NodeHandle should also accept ChildNodeHandle.

ChildNodeHandle is permitted to be the same type as NodeHandle. It may also be
a specialization of ChildNodeIterator.

## Iterators

A node iterator is essentially a wrapper for a NodeHandle with a traversal
behavior baked in. It must be implicitly convertible to/from, and `==` and `!=` 
comparable to the respective node handle. It must be dereferenceable to the 
same value as dereferencing the NodeHandle. Otherwise, they must meet the STL
requirements of LegacyForwardIterator. An iterator is permitted to be "invalid" 
or "valid but not dereferenceable" without being equivalent to a null 
NodeHandle.

## datatree_root

API form:
`aux_datatree::root( Tree&& )`

Free form:
`datatree_node_root( Tree&& )`

Returns a handle to the tree's root node. 

**An overload of this function is required for all custom trees.**

## datatree_node_parent

API form:
`aux_datatree::node_parent( NodeHandle )`

Free form:
`datatree_node_parent( NodeHandle )`

Returns a handle to the node's parent. 

It is highly recommended to provide an overload of this function, if a node's
parent can be determined.

## datatree_node_children_begin

API form:
`aux_datatree::node_children_begin( NodeHandle )`

Free form:
`datatree_node_children_begin( NodeHandle )`

Returns a ChildNodeHandle referring to the first child of the given node. Or if
the given node has no children, then for a NodeHandle `n`, the expression
`aux_datatree::node_children_begin(n) == aux_datatree::ndoe_children_end(n)`
is true.

**An overload of this function is required for all custom node handles.**

## datatree_node_children_end

API form:
`aux_datatree::node_children_end( NodeHandle )`

Free form:
`datatree_node_children_end( NodeHandle )`

Returns an object that is `==` and `!=` comparable to ChildNodeHandle.

For a NodeHandle `n`, and a ChildNodeHandle `c`, if 
`c == aux_datatree::node_children_end(n)` gives true, then:
- `c` has no "next" sibling. Calling `aux_datatree::node_sibling_increment(c)` 
or `aux_datatree::node_sibling_next(c)` give undefined behavior.
- `c` does not refer to a valid child of `n`. Dereferencing `c` gives undefined 
behavior.
- If `c == aux_datatree::node_children_begin(n)` also gives true, then `n` has 
no children.

This is analogous (and possibly equivalent) to `c` being a past-the-end forward
iterator.

**An overload of this function is required for all custom node handles.**

## datatree_node_sibling_next and datatree_node_sibling_increment

API forms:
`aux_datatree::node_sibling_next( ChildNodeHandle )`
`aux_datatree::node_sibling_increment( ChildNodeHandle& )`

Free forms:
`datatree_node_sibling_next( ChildNodeHandle )`
`datatree_node_sibling_increment( ChildNodeHandle& )`

These functions provide complementary forms of the same behavior.

For a ChildNodeHandle `c`, the `..._next` function returns a ChildNodeHandle 
referring to the "next" sibling of `c` (where a node's siblings are modeled as a
forward linear sequence).

For a ChildNodeHandle `c`, the `..._increment` function alters `c` to refer to
its "next" sibling.

Essentially, for a ChildNodeHandle `c`, `aux_datatree::node_sibling_increment( c )` 
is equivalent to `c = aux_datatree::node_sibling_next( std::move(c) )` and
the value returned by `aux_datatree::node_sibling_next( c )` is equivalent to
the value of `c2` in the following example:

```c++
std::decay_t<decltype(c)> c2( c );
aux_datatree::node_sibling_increment(c2);
```

If the free form for only one of these functions is given for a ChildNodeHandle, 
then the other will automatically be defined essentially as above. If both free 
functions are defined, then the custom implementations will be used for both.

To take advantage of possible optimizations and to avoid possible costs of
copying, it is typically preferable to use `aux_datatree::node_sibling_increment`, 
particularly in the typical use case of iterating over a node's children.

**An overload of one or both of these functions is required for all custom node 
handles.** If ChildNodeHandle is a specialization of ChildNodeIterator then the
overload is already given.

## datatree_node_child_count
`datatree_node_child_count( NodeHandle )`

## datatree_node_child_at
`datatree_node_child_at( NodeHandle )`

## datatree_node_index

`datatree_node_index( NodeHandle )`

Returns the index of the node in its parent



## datatree_node_depth

`datatree_node_depth( NodeHandle )`

## datatree_node_root

`datatree_node_root( NodeHandle )`

datatree_node_dfs_end
datatree_node_dfs_next

datatree_node_address

I'm not yet satisfied with the design of data access / structure modification
functions, so they are not yet in this reference.
