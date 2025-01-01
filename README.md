# QueueStucture-AVL-Tree

This repository provides a template class Queue (parameterized by the type of elements stored) with the following functionality:

empty(): Checks whether the queue is empty.
size(): Returns the number of elements in the queue.
push_last(T x): Adds a new element x to the end of the queue and returns a reference (Ref) to that element. This reference remains valid as long as the element is in the queue.
pop_first(): Removes the first element from the queue.
position(Ref ref): Returns the current position of the referenced element in the queue (starting at 0 for the front).
jump_ahead(Ref ref, size_t jmp): Moves the referenced element forward by jmp positions (closer to the front). If jmp is larger than the current position, the element moves to the front of the queue. The reference remains valid even after the move.
The Ref type is an internal class that provides a stable reference to an element in the queue, remaining valid until the element is removed or the queue is destroyed.
