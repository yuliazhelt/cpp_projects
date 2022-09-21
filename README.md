# cpp_projects

## compressed_pair

In this task, an analog of the std::pair class is implemented, which has the following useful property: if one of the stored types is empty, then it does not occupy memory in the structure.

## lru_cache

An associative container that displaces elements according to the Least Recently Used algorithm.

## safe_transform

The TransformIf function, which takes 2 iterators specifying a sequence, a unary predicate p and a functor f, and applies this functor to all elements of the sequence for which the specified predicate is executed. It is assumed that f modifies the passed element (accepts by reference), so this function, unlike std::transform, works in-place.

## strings_operations

Optimal functions for working with strings. According to situation std::string or std::string_view were applied in the arguments and return value for the minimum possible number of allocations without the assumption of optimizing small strings.

## shared_ptr

Own implementation of std::shared_ptr.

## unique_ptr

Own implementation of std::unique_ptr.
