#include <ogdl.h>

#include <fstream>

using namespace ogdl;

class Reader
{
	static constexpr char EOT = 4;
	std::istream& _in;
	char _current;

	char _advance()
	{
		const auto res = _current;
		_in.get(_current);
		if (!_in.good())
			_current = EOT;
		return res;
	}

	size_t _skip_space()
	{
		size_t result = 0;
		while (_current == ' ' || _current == '\t')
		{
			result += 1;
			_advance();
		}
		return result;
	}

	void _handle_CR()
	{
		if (_in.peek() == '\n')
			_advance();
	}

	NodePtr _read_node()
	{
		NodePtr result;
		{
			std::string name;
			_skip_space();
			switch (_current)
			{
			case '\r': _handle_CR();
			case '\n': 
			case EOT:
				break;
			case '\'':
			case '"':
			{
				const auto delim = _advance();
				while (_current != delim && _current >= ' ')
					name.push_back(_advance());
				if (_current != delim)
					throw std::runtime_error(std::string("expected string delimiter ") + char(delim));
				_advance();
				break;
			}
			default:
				while (_current > ' ' && _current != ',')
					name.push_back(_advance());
				break;
			}
			if (name.empty())
				return nullptr;
			result = Node::make(std::move(name));
		}

		for (;;) switch (_current)
		{
		case '\r': _handle_CR(); // fallthrough
		case '\n': 
		case ',':
		case EOT:
			return result;
		case ' ': case '\t': 
			_advance();
			continue;
		default:
			if (_current < ' ')
				return result;
			result->push(_read_node());
			continue;
		}
	}

	struct Partial
	{
		size_t next_indent = 0;
		std::vector<NodePtr> nodes;
	};
	Partial _read_sub(const size_t indent)
	{
		Partial result;
		for (;;)
		{
			if (const auto node = _read_node())
			{
				result.nodes.emplace_back(node);
				switch (_current)
				{
				case ',':
					_advance();
					continue;
				case '\r': _handle_CR(); // fallthrough
				case '\n':
				{
					_advance();
					result.next_indent = _skip_space();
					if (result.next_indent > indent)
					{
						auto sub = _read_sub(result.next_indent);
						for (auto&& n : sub.nodes)
							result.nodes.back()->push(std::move(n));
						result.next_indent = sub.next_indent;
					}

					if (result.next_indent == indent)
						continue;
					else
						break;
				}
				case EOT:
					result.next_indent = 0;
					break;
				default:
					throw std::runtime_error("unexpected character after reading node");
				}

			}
			return result;
		}
	}
public:
	Reader(std::istream& in) : _in(in) { _advance(); }

	std::vector<NodePtr> read()
	{
		return _read_sub(0).nodes;
	}
};

std::vector<NodePtr> ogdl::read(std::istream& in)
{
	return Reader(in).read();
}