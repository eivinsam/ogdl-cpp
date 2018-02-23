#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>

namespace ogdl
{

	class Node;
	class NodePtr
	{
		friend class Node;

		std::shared_ptr<Node> ptr;

		NodePtr(std::shared_ptr<Node> ptr) : ptr(std::move(ptr)) { }
	public:
		NodePtr() = default;
		NodePtr(nullptr_t) : ptr() { }

		Node* operator->() const { return ptr.get(); }
		Node& operator*() const { return *ptr; }

		NodePtr operator[](size_t index) const;

		explicit operator bool() const { return bool(ptr); }
	};

	class Node
	{
		std::vector<NodePtr> _edges;
	public:
		std::string name;

		Node(std::string name) : name(std::move(name)) { }

		static NodePtr make(std::string name) { return { std::make_shared<Node>(std::move(name)) }; }

		void push(NodePtr edge) { _edges.emplace_back(std::move(edge)); }

		NodePtr operator[](size_t index) const
		{
			return index < _edges.size() ? _edges[index] : nullptr;
		}

		auto& edges() const { return _edges; }

		void write(std::ostream& out) const;
		void write(std::ostream&& out) const { write(out); }
	};


	std::vector<NodePtr> read(std::istream& in);	
	inline std::vector<NodePtr> read(std::istream&& in) { return read(in); }
}
