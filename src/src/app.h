#pragma once

#include <array>
#include <vector>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "config.h"
#include "trees.h"
#include "scc.h"


namespace app
{
	// Settings
	extern bool showGrids;
	extern int nodeSpacing;

	// Displayed windows 
	extern bool displaySettings, displayNodeActions, displayNodeInfo, displayCanvasInfo;
	extern bool displayInsertNode, displayInsertRandNodes;	// Popups

	// Input params
	extern int inputNodeValue, inputPriorNodeValue, inputNodesCountValue;
	extern trees::Trees selectedTree;

	// Trees
	extern trees::AVLTree avl;
	extern trees::RBTree rb;
	extern trees::Treap treap;
	extern trees::SplayTree splay;

	// Canvas vars
	extern scc::Canvas canvas;
	extern std::vector<trees::CanvasNode> canvasNodes;
	extern bool movingCanvas, buildNewTree;
	extern sf::Vector2f savedCursor;
	extern size_t hoveredNode;

	// Grid vars
	extern const sf::Color gridlineColor, gridlineAltColor;
	extern const float gridlineThickness;
	extern sf::RectangleShape horizontalLineA, horizontalLineB, verticalLineA, verticalLineB;

	// Other
	extern sf::Font font;
	extern sf::Image logo;

	// Tree logic & display
	const trees::Node* getCurrentTreeRoot();
	void _calculateTree(const trees::Node* tree, const auxillary::vec2& c, bool init = false);
	void calculateTree();
	void drawNode(sf::RenderWindow* window, const trees::Node* node, size_t i);
	void _drawTree(sf::RenderWindow* window, const trees::Node* tree, size_t& i);
	void drawTree(sf::RenderWindow* window);

	// Grid
	void setupGridLines();
	void drawGrid(sf::RenderWindow* window);

	// Tree operations
	void insertNode();
	void insertRandomNodes();
	void eraseNode();

	// Events
	void handleWindowEvents(sf::Window* window);
	void handleEvent(sf::Window* window, const sf::Event&& event);

	// Popups
	void showInsertNodePopup();
	void closeInsertNodePopup(bool abort = false);
	void showInsertRandomNodesPopup();
	void closeInsertRandomNodesPopup(bool abort = false);

	// Windows
	void showSettingsWindow();
	void showNodeInfoWindow();
	void showNodeActionsWindow();
	void showCanvasInfoWindow(sf::Window* window);
}
