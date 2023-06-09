#pragma once

#include <SFML/Graphics.hpp>

#include <utility>
#include <random>
#include <ctime>

#include "auxillary.h"
#include "scc.h"


namespace trees
{
    enum class Trees
    {
        AVL, RB, Treap, Splay
    };

    extern const std::array<Trees, 4> TreesIter;

    const char* treeToString(Trees tree);

    // Base class for all nodes
    class Node
    {
    public:
        static sf::Font font;
        static float diameter, spacing, outlineThickness;  // in canvas units

        size_t h, n;
        size_t elem;
        Node* parent, * l, * r;

        Node(size_t elem, Node* parent = nullptr, size_t h = 1, size_t n = 1, Node* l = nullptr, Node* r = nullptr);

        virtual void draw(sf::RenderWindow* window, const scc::Canvas& canvas, 
                          auxillary::vec2 coordinate, sf::FloatRect* outBoundary) const;

        void update();
    };

    // Class for storing displayed nodes
    struct CanvasNode
    {
        const Node* const node;
        const auxillary::BoundingBox box;

        CanvasNode(const Node* node, const auxillary::BoundingBox& box);

        bool contains(const auxillary::vec2& v) const;
    };

    // Base class for all trees
    class Tree
    {
    public:
        using NodeType = Node;
    protected:
        NodeType* tree;

        static void leftRotate(NodeType*& node);
        static void rightRotate(NodeType*& node);
        static NodeType* findNearestLT(const NodeType* node);
        static NodeType* findNearestGT(const NodeType* node);
    public:
        Tree(NodeType* tree = nullptr);

        virtual const NodeType* insert(size_t val) = 0;
        void insertRandom(size_t n);
        virtual bool erase(size_t val) = 0;

        const NodeType* rootPtr() const;
    };


    class AVLTree : public Tree
    {
    public:
        using NodeType = Node;
    private:
        enum class BalancingTypes
        {
            None, Left, Right, LargeLeft, LargeRight
        };

        static BalancingTypes checkBalance(const NodeType* node);
        static NodeType* balanceUp(NodeType*& node);
    public:
        AVLTree();

        const NodeType* insert(size_t val) override;
        bool erase(size_t val) override;
    };

    class RBTree : public Tree
    {
    private:
        class RBNode : public Node
        {
        public:
            bool red;

            RBNode(size_t elem, RBNode* parent = nullptr, bool red = true, size_t h = 1, size_t n = 1,
                RBNode* l = nullptr, RBNode* r = nullptr);

            void draw(sf::RenderWindow* window, const scc::Canvas& canvas,
                      auxillary::vec2 coordinate, sf::FloatRect* outBoundary) const override;
        };
    public:
        using NodeType = RBNode;
    private:
        static NodeType*& ptrCast(Tree::NodeType*& node);
        static NodeType* grandparent(const Tree::NodeType* node);
        static NodeType* uncle(const Tree::NodeType* node);

        static NodeType* insertBalance(NodeType*& node);
        static void deleteBlackLeaf(NodeType* node);

        static void updateTree(NodeType* node);
    public:
        RBTree();

        const NodeType* insert(size_t val) override;
        bool erase(size_t val) override;
    };

    class Treap : public Tree
    {
    private:
        static std::mt19937 rng;

        class TreapNode : public Node
        {
        public:
            size_t prior;

            TreapNode(size_t elem, size_t prior = -1, TreapNode* parent = nullptr, size_t h = 1, 
                size_t n = 1, TreapNode* l = nullptr, TreapNode* r = nullptr);

            void draw(sf::RenderWindow* window, const scc::Canvas& canvas,
                      auxillary::vec2 coordinate, sf::FloatRect* outBoundary) const override;
        };
    public:
        using NodeType = TreapNode;
    private:
        static NodeType* merge(NodeType* l, NodeType* r);
        static void split(NodeType* tree, size_t key, NodeType*& l, NodeType*& r);
    public:
        Treap();

        const NodeType* insert(size_t val) override;
        const NodeType* insert(size_t val, size_t prior);
        bool erase(size_t val) override;
    };

    class SplayTree : public Tree
    {
    private:
        static NodeType* splay(NodeType*& node);
        static void zig(NodeType*& node);
        static void zigzig(NodeType*& node);
        static void zigzag(NodeType*& node);
    public:
        using NodeType = Node;

        SplayTree();

        const NodeType* insert(size_t val) override;
        bool erase(size_t val) override;
    };
}
