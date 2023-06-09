#include "trees.h"


namespace trees
{
	const std::array<Trees, 4> TreesIter = {
		Trees::AVL, Trees::RB, Trees::Treap, Trees::Splay
	};

	const char* treeToString(Trees tree)
	{
		if (tree == Trees::AVL)
			return "AVL";
		if (tree == Trees::RB)
			return "RB";
		if (tree == Trees::Treap)
			return "Treap";
		return "Splay";
	}

	#pragma region Node
	float Node::diameter = 1.f, Node::spacing = .4f, Node::outlineThickness = 2.f;
	sf::Font Node::font;

	Node::Node(size_t elem, Node* parent, size_t h, size_t n, Node* l, Node* r)
		: elem(elem), parent(parent), h(h), n(n), l(l), r(r) {}

	void Node::update()
	{
		h = std::max(l == nullptr ? 0 : l->h, r == nullptr ? 0 : r->h) + 1;
		n = (l == nullptr ? 0 : l->n) + (r == nullptr ? 0 : r->n) + 1;
	}

	void Node::draw(sf::RenderWindow* window, const scc::Canvas& canvas, 
					auxillary::vec2 coordinate, sf::FloatRect* outBoundary) const
	{
		const float r = canvas.canvasDistToPixel(diameter / 2.f);
		sf::CircleShape node(r);
		node.setOutlineThickness(outlineThickness);
		node.setOutlineColor(sf::Color::Black);
		node.setOrigin(sf::Vector2f(r, r));
		std::string s = std::to_string(elem);
		sf::Text text(s, font, (unsigned)auxillary::lerp<double, double>(canvas.width, 10, 60., 16., 4.));
		if (text.getGlobalBounds().width > .8 * canvas.canvasDistToPixel(diameter))
			text.setString(s.substr(0, 5) + '#');
		sf::FloatRect textGlobalBounds = text.getGlobalBounds(),
			textLocalBounds = text.getLocalBounds();
		text.setFillColor(sf::Color::Black);
		text.setOrigin(auxillary::round(sf::Vector2f(
			textGlobalBounds.width / 2 + textLocalBounds.left,
			textGlobalBounds.height / 2 + textLocalBounds.top
		)));
		sf::Vector2f screenPos = canvas.canvasPosToPixel(coordinate);
		node.setPosition(screenPos);
		text.setPosition(screenPos);
		window->draw(node);
		window->draw(text);
		*outBoundary = node.getGlobalBounds();
	}
	#pragma endregion

	#pragma region CanvasNode
	CanvasNode::CanvasNode(const trees::Node* node, const auxillary::BoundingBox& box)
		: node(node), box(box) {}

	bool CanvasNode::contains(const auxillary::vec2& v) const
	{
		auxillary::vec2 delta = box.center - v;
		return delta.x * delta.x + delta.y * delta.y <= box.width * box.width / 4.;
	}
	#pragma endregion

	#pragma region Tree
	Tree::Tree(NodeType* tree) : tree(tree) {}

	void Tree::leftRotate(NodeType*& node)
	{
		NodeType* p = node, * q = node->l;
		p->l = q->r;
		if (q->r != nullptr)
			p->l->parent = p;
		if ((q->parent = p->parent) != nullptr)
			(p->parent->l == p ? p->parent->l = q : p->parent->r = q);
		q->r = p, p->parent = q;
		p->update();
		q->update();
		if (q->parent != nullptr) 
			q->parent->update();
		node = q;
	}

	void Tree::rightRotate(NodeType*& node)
	{
		NodeType* p = node, * q = node->r;
		p->r = q->l;
		if (q->l != nullptr) 
			p->r->parent = p;
		if ((q->parent = p->parent) != nullptr)
			(p->parent->l == p ? p->parent->l = q : p->parent->r = q);
		q->l = p, p->parent = q;
		p->update();
		q->update();
		if (q->parent != nullptr)
			q->parent->update();
		node = q;
	}

	Tree::NodeType* Tree::findNearestLT(const NodeType* node)
	{
		NodeType* p = node->l;
		while (p != nullptr && p->r != nullptr)
			p = p->r;
		return p;
	}

	Tree::NodeType* Tree::findNearestGT(const NodeType* node)
	{
		NodeType* p = node->r;
		while (p != nullptr && p->l != nullptr)
			p = p->l;
		return p;
	}

	void Tree::insertRandom(size_t n)
	{
		static std::mt19937 rng((unsigned)std::time(nullptr));
		while (n)
		{
			if (insert(rng()) != nullptr)
				--n;
		}
	}

	const Tree::NodeType* Tree::rootPtr() const
	{
		return tree;
	}
	#pragma endregion

	#pragma region AVL
	AVLTree::AVLTree() : Tree() {}

	AVLTree::BalancingTypes AVLTree::checkBalance(const NodeType* node)
	{
		static auto h = [](const NodeType* node) { return (long long)(node == nullptr ? 0 : node->h); };
		if (h(node->l) - h(node->r) > 1)
		{
			if (h(node->l->r) <= h(node->l->l))
				return BalancingTypes::Left;
			return BalancingTypes::LargeLeft;
		}
		else if (h(node->l) - h(node->r) < -1)
		{
			if (h(node->r->l) <= h(node->r->r))
				return BalancingTypes::Right;
			return BalancingTypes::LargeRight;
		}
		return BalancingTypes::None;
	}

	AVLTree::NodeType* AVLTree::balanceUp(NodeType*& node)
	{
		NodeType* p = node;
		while (p != nullptr)
		{
			p->update();
			BalancingTypes balanceType = checkBalance(p);
			if (balanceType == BalancingTypes::Left)
				leftRotate(p);
			else if (balanceType == BalancingTypes::Right)
				rightRotate(p);
			else if (balanceType == BalancingTypes::LargeLeft)
				rightRotate(p->l), leftRotate(p);
			else if (balanceType == BalancingTypes::LargeRight)
				leftRotate(p->r), rightRotate(p);
			node = p, p = p->parent;
		}
		return node;
	}

	const AVLTree::NodeType* AVLTree::insert(size_t val)
	{
		if (tree == nullptr)
		{
			tree = new NodeType(val);
			return tree;
		}
		NodeType* p = tree, * ret;
		while (val == p->elem || p->l != nullptr && val < p->elem || p->r != nullptr && val > p->elem)
		{
			if (val == p->elem)
				return nullptr;
			if (val > p->elem)
				p = p->r;
			else
				p = p->l;
		}
		ret = (val > p->elem ? p->r = new NodeType(val, p) : p->l = new NodeType(val, p));
		tree = balanceUp(p);
		return ret;
	}

	bool AVLTree::erase(size_t val)
	{
		static auto h = [](const NodeType* node) { return (long long)(node == nullptr ? 0 : node->h); };
		if (tree == nullptr)
			return false;
		NodeType* p = tree;
		while (p != nullptr && p->elem != val)
			p = (p->elem < val ? p->r : p->l);
		if (p == nullptr)
			return false;
		while (p->l != nullptr || p->r != nullptr)
		{
			NodeType* q = (h(p->l) > h(p->r) ? findNearestLT(p) : findNearestGT(p));
			std::swap(p->elem, q->elem);
			p = q;
		}
		if (p->parent == nullptr)
		{
			delete tree, tree = nullptr;
			return true;
		}
		bool leftSon = (p->parent->l == p);
		p = p->parent;
		if (leftSon)
			delete p->l, p->l = nullptr;
		else
			delete p->r, p->r = nullptr;
		tree = balanceUp(p);
		return true;
	}
	#pragma endregion

	#pragma region RB
	RBTree::RBNode::RBNode(size_t elem, RBNode* parent, bool red, size_t h, size_t n, RBNode* l, RBNode* r)
		: Node(elem, parent, h, n, l, r), red(red) {}

	void RBTree::RBNode::draw(sf::RenderWindow* window, const scc::Canvas& canvas,
							  auxillary::vec2 coordinate, sf::FloatRect* outBoundary) const
	{
		const float r = canvas.canvasDistToPixel(diameter / 2);
		sf::CircleShape node(r);
		node.setOutlineThickness(outlineThickness);
		node.setOutlineColor(sf::Color::Black);
		node.setOrigin(sf::Vector2f(r, r));
		std::string s = std::to_string(elem);
		sf::Text text(s, font, (unsigned)auxillary::lerp<double, double>(canvas.width, 10, 60., 16., 4.));
		if (text.getGlobalBounds().width > .8 * canvas.canvasDistToPixel(diameter))
			text.setString(s.substr(0, 5) + '#');
		sf::FloatRect textGlobalBounds = text.getGlobalBounds(),
			textLocalBounds = text.getLocalBounds();
		text.setFillColor(sf::Color::White);
		text.setOrigin(auxillary::round(sf::Vector2f(
			textGlobalBounds.width / 2 + textLocalBounds.left,
			textGlobalBounds.height / 2 + textLocalBounds.top
		)));
		sf::Vector2f screenPos = canvas.canvasPosToPixel(coordinate);
		if (red)
			node.setFillColor(sf::Color(0xfe5e41ff));
		else
			node.setFillColor(sf::Color(0x828a95ff));
		node.setPosition(screenPos);
		text.setPosition(screenPos);
		window->draw(node);
		window->draw(text);
		*outBoundary = node.getGlobalBounds();
	}

	RBTree::RBTree() : Tree() {}

	RBTree::NodeType*& RBTree::ptrCast(Tree::NodeType*& node)
	{
		return (NodeType*&)node;
	}

	RBTree::NodeType* RBTree::grandparent(const Tree::NodeType* node)
	{
		if (node->parent != nullptr)
			return (NodeType*)node->parent->parent;
		return nullptr;
	}

	RBTree::NodeType* RBTree::uncle(const Tree::NodeType* node)
	{
		NodeType* gp = grandparent(node);
		if (gp != nullptr)
			return (NodeType*)(node->parent == gp->l ? gp->r : gp->l);
		return nullptr;
	}
	
	RBTree::NodeType* RBTree::insertBalance(NodeType*& node)
	{
		NodeType* gp = grandparent(node), * u = uncle(node);
		node->update();
		if (node->parent == nullptr)
		{
			node->red = false;
			return node;
		}
		else if (!(ptrCast(node->parent)->red))
		{
			return node;
		}
		else if (u != nullptr && u->red)
		{
			ptrCast(node->parent)->red = false, u->red = false;
			gp->red = true;
			return insertBalance(gp), gp;
		}
		else if (node == node->parent->l && node->parent == gp->r || 
			node == node->parent->r && node->parent == gp->l)
		{
			if (node == node->parent->l)
			{
				node = ptrCast(node->parent);
				leftRotate((Tree::NodeType*&)node);
				node = ptrCast(node->r);
			}
			else
			{
				node = ptrCast(node->parent);
				rightRotate((Tree::NodeType*&)node);
				node = ptrCast(node->l);
			}
		}
		ptrCast(node->parent)->red = false;
		gp->red = true;
		if (node == node->parent->l && node->parent == gp->l)
			leftRotate((Tree::NodeType*&)gp);
		else
			rightRotate((Tree::NodeType*&)gp);
		return node;
	}

	void RBTree::deleteBlackLeaf(NodeType* node)
	{
		NodeType* p = ptrCast(node->parent), * s, * c, * d, * toDelete = node;
		bool leftChild;
		if (leftChild = node == p->l)
			p->l = nullptr;
		else
			p->r = nullptr;
		goto start;
		do
		{
			leftChild = (node == node->parent->l);
		start:
			s = ptrCast(leftChild ? p->r : p->l);
			d = ptrCast(leftChild ? s->r : s->l);
			c = ptrCast(leftChild ? s->l : s->r);
			if (s->red)
				goto case3;
			if (d != nullptr && d->red)
				goto case6;
			if (c != nullptr && c->red)
				goto case5;
			if (p->red)
				goto case4;
			s->red = true;
			node = p;
		} while ((p = ptrCast(node->parent)) != nullptr);
		delete toDelete;
		return;
	case3:
		if (leftChild)
			rightRotate((Tree::NodeType*&)p), p = ptrCast(p->l);
		else
			leftRotate((Tree::NodeType*&)p), p = ptrCast(p->r);
		p->red = true;
		s->red = false;
		s = c;
		d = ptrCast(leftChild ? s->r : s->l);
		if (d != nullptr && d->red)
			goto case6;
		c = ptrCast(leftChild ? s->l : s->r);
		if (c != nullptr && c->red)
			goto case5;
	case4:
		s->red = true;
		p->red = false;
		delete toDelete;
		return;
	case5:
		if (leftChild)
			leftRotate((Tree::NodeType*&)s), s = ptrCast(s->r);
		else
			rightRotate((Tree::NodeType*&)s), s = ptrCast(s->l);
		s->red = true;
		c->red = false;
		d = s;
		s = c;
	case6:
		if (leftChild)
			rightRotate((Tree::NodeType*&)p), p = ptrCast(p->l);
		else
			leftRotate((Tree::NodeType*&)p), p = ptrCast(p->r);
		s->red = p->red;
		p->red = false;
		d->red = false;
	}

	void RBTree::updateTree(NodeType* node)
	{
		if (node->l == nullptr && node->r == nullptr)
			return node->update();
		if (node->l != nullptr)
			updateTree(ptrCast(node->l));
		if (node->r != nullptr)
			updateTree(ptrCast(node->r));
		node->update();
	}

	const RBTree::NodeType* RBTree::insert(size_t val)
	{
		if (tree == nullptr)
		{
			tree = new NodeType(val);
			return insertBalance(ptrCast(tree));
		}
		NodeType* p = ptrCast(tree), * ret;
		while (val == p->elem || p->l != nullptr && val < p->elem || p->r != nullptr && val > p->elem)
		{
			if (val == p->elem)
				return nullptr;
			if (val > p->elem)
				p = ptrCast(p->r);
			else
				p = ptrCast(p->l);
		}
		ret = p = ptrCast((val > p->elem ? p->r = new NodeType(val, p) : p->l = new NodeType(val, p)));
		insertBalance(p);
		while (p != nullptr)
			p->update(), tree = p, p = ptrCast(p->parent);
		return ret;
	}

	bool RBTree::erase(size_t val)
	{
		if (tree == nullptr)
			return false;
		NodeType* p = ptrCast(tree);
		while (p != nullptr && p->elem != val)
			p = ptrCast(p->elem < val ? p->r : p->l);
		if (p == nullptr)
			return false;
		if (p->l != nullptr && p->r != nullptr)
		{
			NodeType* q = ptrCast(p->r);
			while (q->l != nullptr)
				q = ptrCast(q->l);
			std::swap(p->elem, q->elem);
			p = q;
		}
		if (p->parent == nullptr && p->l == nullptr && p->r == nullptr)
		{
			delete p, tree = nullptr;
			return true;
		}
		if (p->red && p->l == nullptr && p->r == nullptr)
		{
			if (p == p->parent->l)
				p = ptrCast(p->parent), delete p->l, p->l = nullptr;
			else
				p = ptrCast(p->parent), delete p->r, p->r = nullptr;
			while (p != nullptr)
				p->update(), p = ptrCast(p->parent);
			return true;
		}
		if (!p->red && ((p->l == nullptr) ^ (p->r == nullptr)))
		{
			if (p->l == nullptr)
			{
				p->elem = p->r->elem;
				delete p->r, p->r = nullptr;
			}
			else
			{
				p->elem = p->l->elem;
				delete p->l, p->l = nullptr;
			}
			while (p != nullptr)
				p->update(), p = ptrCast(p->parent);
			return true;
		}
		NodeType* q = ptrCast(p->parent);
		deleteBlackLeaf(p);
		while (q != nullptr)
			q->update(), q = ptrCast(q->parent);
		while (tree->parent != nullptr)
			tree = tree->parent;
		//updateTree(ptrCast(tree));
		return true;
	}
	#pragma endregion

	#pragma region Treap
	std::mt19937 Treap::rng((unsigned)std::time(nullptr));

	Treap::TreapNode::TreapNode(size_t elem, size_t prior, TreapNode* parent, size_t h, size_t n, 
		TreapNode* l, TreapNode* r) 
		: Node(elem, parent, h, n, l, r), prior(prior)
	{
		if (prior == -1)
			this->prior = rng();
	}

	void Treap::TreapNode::draw(sf::RenderWindow* window, const scc::Canvas& canvas,
								auxillary::vec2 coordinate, sf::FloatRect* outBoundary) const
	{
		const float r = canvas.canvasDistToPixel(diameter / 2);
		sf::CircleShape node(r);
		node.setOutlineThickness(outlineThickness);
		node.setOutlineColor(sf::Color::Black);
		node.setOrigin(sf::Vector2f(r, r));
		std::string val_s = std::to_string(elem), prior_s = std::to_string(prior);
		sf::Text value(
				val_s, font, 
				(unsigned)auxillary::lerp<double, double>(canvas.width, 10, 60., 16., 3.)
			),
			priority(
				prior_s, font, 
				(unsigned)auxillary::lerp<double, double>(canvas.width, 10, 60., 12., 2.)
			);
		if (value.getGlobalBounds().width > .7 * canvas.canvasDistToPixel(diameter))
			value.setString(val_s.substr(0, 5) + '#');
		if (priority.getGlobalBounds().width > .7 * canvas.canvasDistToPixel(diameter))
			priority.setString(prior_s.substr(0, 5) + '#');
		sf::FloatRect valueGlobalBounds = value.getGlobalBounds(),
			valueLocalBounds = value.getLocalBounds(),
			priorGlobalBounds = priority.getGlobalBounds(),
			priorLocalBounds = priority.getLocalBounds();
		value.setFillColor(sf::Color::Black), priority.setFillColor(sf::Color(0x8a8d91ff));
		value.setOrigin(auxillary::round(sf::Vector2f(
			valueGlobalBounds.width / 2 + valueLocalBounds.left,
			valueGlobalBounds.height / 2 + valueLocalBounds.top
		)));
		priority.setOrigin(auxillary::round(sf::Vector2f(
			priorGlobalBounds.width / 2 + priorLocalBounds.left,
			priorGlobalBounds.height / 2 + priorLocalBounds.top
		)));
		sf::Vector2f screenPos = canvas.canvasPosToPixel(coordinate);
		node.setPosition(screenPos);
		value.setPosition(screenPos - sf::Vector2f(0.f, (float)value.getCharacterSize() / 2.f));
		priority.setPosition(screenPos + sf::Vector2f(0.f, (float)value.getCharacterSize() / 2.f));
		window->draw(node);
		window->draw(value);
		window->draw(priority);
		*outBoundary = node.getGlobalBounds();
	}

	Treap::Treap() : Tree() {}

	Treap::NodeType* Treap::merge(NodeType* l, NodeType* r)
	{
		if (l == nullptr || r == nullptr)
			return l == nullptr ? r : l;
		if (l->prior > r->prior)
		{
			l->r = merge((NodeType*)l->r, r);
			if (l->r != nullptr)
				l->r->parent = l, l->r->update();
			l->update();
			return l;
		}
		else
		{
			r->l = merge(l, (NodeType*)r->l);
			if (r->l != nullptr)
				r->l->parent = r, r->l->update();
			r->update();
			return r;
		}
	}

	void Treap::split(NodeType* tree, size_t key, NodeType*& l, NodeType*& r)
	{
		if (tree == nullptr)
		{
			l = r = nullptr;
			return;
		}
		if (tree->elem < key)
		{
			split((NodeType*)tree->r, key, (NodeType*&)tree->r, r);
			l = tree;
			if (tree->r != nullptr)
				tree->r->parent = l, tree->r->update();
			l->update();
		}
		else
		{
			split((NodeType*)tree->l, key, l, (NodeType*&)tree->l);
			r = tree;
			if (tree->l != nullptr)
				tree->l->parent = r, tree->l->update();
			r->update();
		}
		if (l != nullptr)
			l->parent = nullptr;
		if (r != nullptr)
			r->parent = nullptr;
	}

	const Treap::NodeType* Treap::insert(size_t val, size_t prior)
	{
		NodeType* l, * r;
		split((NodeType*)tree, val, l, r);
		tree = merge(merge(l, new NodeType(val, prior)), r);
		return (NodeType*)tree;
	}

	const Treap::NodeType* Treap::insert(size_t val)
	{
		NodeType* l, * r, * m, * node;
		split((NodeType*)tree, val, l, r);
		split(r, val + 1, m, r);
		if (m != nullptr)
		{
			tree = merge(l, merge(m, r));
			return nullptr;
		}
		node = new NodeType(val);
		tree = merge(merge(l, node), r);
		return (NodeType*)tree;
	}

	bool Treap::erase(size_t val)
	{
		if (tree == nullptr)
			return false;
		NodeType* l, * r, * m;
		split((NodeType*)tree, val, l, r);
		split(r, val + 1, m, r);
		tree = merge(l, r);
		delete m;
		return true;
	}
	#pragma endregion

	#pragma region Splay
	SplayTree::SplayTree() : Tree() {}

	void SplayTree::zig(NodeType*& node)
	{
		if (node == node->parent->l)
			node = node->parent, leftRotate(node);
		else
			node = node->parent, rightRotate(node);
	}

	void SplayTree::zigzig(NodeType*& node)
	{
		if (node == node->parent->l)
			node = node->parent, leftRotate(node), node = node->parent, leftRotate(node);
		else
			node = node->parent, rightRotate(node), node = node->parent, rightRotate(node);
	}

	void SplayTree::zigzag(NodeType*& node)
	{
		if (node == node->parent->l)
			node = node->parent, leftRotate(node), node = node->parent, rightRotate(node);
		else
			node = node->parent, rightRotate(node), node = node->parent, leftRotate(node);
	}

	SplayTree::NodeType* SplayTree::splay(NodeType*& node)
	{
		while (node->parent != nullptr)
		{
			if (node->parent->parent == nullptr)
				zig(node);
			else if (node == node->parent->l)
			{
				if (node->parent == node->parent->parent->l)
					zigzig(node);
				else
					zigzag(node);
			}
			else
			{
				if (node->parent == node->parent->parent->l)
					zigzag(node);
				else
					zigzig(node);
			}
		}
		return node;
	}

	const SplayTree::NodeType* SplayTree::insert(size_t val)
	{
		if (tree == nullptr)
		{
			tree = new NodeType(val);
			return tree;
		}
		NodeType* p = tree;
		while (val == p->elem || p->l != nullptr && val < p->elem || p->r != nullptr && val > p->elem)
		{
			if (val == p->elem)
				return nullptr;
			if (val > p->elem)
				p = p->r;
			else
				p = p->l;
		}
		if (val > p->elem)
			p = p->r = new NodeType(val, p), p->parent->update();
		else
			p = p->l = new NodeType(val, p), p->parent->update();
		tree = splay(p);
		return p;
	}

	bool SplayTree::erase(size_t val)
	{
		static auto n = [](const NodeType* node) { return (long long)(node == nullptr ? 0 : node->n); };
		if (tree == nullptr)
			return false;
		NodeType* p = tree;
		while (p != nullptr && p->elem != val)
			p = (p->elem < val ? p->r : p->l);
		if (p == nullptr)
			return false;
		while (p->l != nullptr || p->r != nullptr)
		{
			NodeType* q = (n(p->l) > n(p->r) ? findNearestLT(p) : findNearestGT(p));
			std::swap(p->elem, q->elem);
			p = q;
		}
		if (p->parent == nullptr)
		{
			delete tree, tree = nullptr;
			return true;
		}
		bool leftSon = (p->parent->l == p);
		p = p->parent;
		if (leftSon)
			delete p->l, p->l = nullptr;
		else
			delete p->r, p->r = nullptr;
		tree = splay(p);
		return true;
	}
	#pragma endregion
}
