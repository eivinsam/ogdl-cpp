#include <ogdl.h>

using namespace ogdl;

struct Writer
{
	std::ostream& out;

	void write(const Node* n, size_t indent)
	{
		static constexpr auto end = std::string::npos;
		for (size_t i = 0; i < indent; ++i)
			out.put(' ');
		if (n->name.find_first_of(" ,#\n\r\t") != end)
		{
			// TODO: check for best quotes, sanitize
			out << '"' << n->name << '"';
		}
		else
			out << n->name;
		out.put('\n');
		for (auto&& e : n->edges())
			write(&*e, indent + 2);
	}
};


void ogdl::Node::write(std::ostream& out) const
{
	Writer{ out }.write(this, 0);
}
