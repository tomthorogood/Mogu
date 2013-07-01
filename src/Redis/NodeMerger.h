#ifndef NODEMERGER_H_
#define NODEMERGER_H_

// If two nodes of different prefixes share the same node name,
// the node merger will merge their data into a new, temporary
// node in the "temp" prefix.
// By default, the temporary node will be deleted when its
// NodeMerger object goes out of scope, or is deleted. However
// this behavior can be altered if one wishes to cache temporary
// nodes for faster lookup at a later time.

class NodeMerger {
public:

private:

    std::string node_name;

}

#endif
