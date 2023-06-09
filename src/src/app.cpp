#include "app.h"


namespace app
{
	// Settings
	bool showGrids = true, spaceEvenly = false;
	int nodeSpacing = 40;

	// Displayed windows
	bool displaySettings = true, displayNodeActions = true, 
		displayNodeInfo = true, displayCanvasInfo = true;
	bool displayInsertNode = false, displayInsertRandNodes = false;	// Popups
	bool canInsertNode = true, canInsertRandNodes = true;	// Popup finishers

	// Input params
	int inputNodeValue = 0, inputNodePriorValue = -1, inputNodesCountValue = 0;
	trees::Trees selectedTree = trees::Trees::AVL;

	// Trees
	trees::AVLTree avl;
	trees::RBTree rb;
	trees::Treap treap;
	trees::SplayTree splay;

	// Canvas vars
	scc::Canvas canvas(auxillary::vec2(-10., 2.), 20.);
	std::vector<trees::CanvasNode> canvasNodes;
	bool movingCanvas = false, buildNewTree = false;
	sf::Vector2f savedCursor;
	size_t hoveredNode = -1;

	// Grid vars
	const sf::Color gridlineColor(0xe5e5e5ff), gridlineAltColor(0x6d6875ff);
	const float gridlineThickness = 1.f;
	sf::RectangleShape horizontalLineA, horizontalLineB, verticalLineA, verticalLineB;

	// Other
	sf::Font font;
	sf::Image logo;

	const trees::Node* getCurrentTreeRoot()
	{
		if (selectedTree == trees::Trees::AVL)
			return avl.rootPtr();
		else if (selectedTree == trees::Trees::RB)
			return rb.rootPtr();
		else if (selectedTree == trees::Trees::Treap)
			return treap.rootPtr();
		else if (selectedTree == trees::Trees::Splay)
			return splay.rootPtr();
		return nullptr;
	}

	void _calculateTree(const trees::Node* node, const auxillary::vec2& c, bool init)
	{
		static std::map<const trees::Node*, float> widths;
		static std::function<float(const trees::Node*)> width = [&](const trees::Node* node)
		{
			if (node == nullptr)
				return 0.f;
			if (widths.find(node) != widths.end())
				return widths[node];
			if (node->l == nullptr && node->r == nullptr)
				return widths[node] = node->diameter + node->spacing;
			if (node->r == nullptr)
				return widths[node] = width(node->l) + .5f * (node->diameter + node->spacing);
			if (node->l == nullptr)
				return widths[node] = width(node->r) + .5f * (node->diameter + node->spacing);
			return widths[node] = width(node->l) + width(node->r);
		};
		if (init)
			widths.clear(), width(node);
		canvasNodes.push_back(
			trees::CanvasNode(
				node, auxillary::BoundingBox::CreateFromCenter(c, { node->diameter, node->diameter })
			)
		);
		if (node->l != nullptr)
		{
			_calculateTree(
				node->l, c - auxillary::vec2(
					widths[node->l->r] == 0.f ? .5f * (node->diameter + node->spacing) : widths[node->l->r],
					node->diameter + node->spacing
				)
			);
		}
		if (node->r != nullptr)
		{
			_calculateTree(
				node->r, c - auxillary::vec2(
					-(widths[node->r->l] == 0.f ? .5f * (node->diameter + node->spacing) : widths[node->r->l]),
					node->diameter + node->spacing
				)
			);
		}
	}

	void calculateTree()
	{
		const trees::Node* tree = getCurrentTreeRoot();
		if (tree != nullptr)
		{
			canvasNodes.clear(), canvasNodes.reserve(tree->n);
			_calculateTree(tree, { 0., 0. }, true);
		}
	}

	void drawNode(sf::RenderWindow* window, const trees::Node* node, size_t i)
	{
		const auxillary::BoundingBox& box = canvasNodes[i].box;
		if (!canvas.view.overlaps(box))
			return;
		sf::FloatRect boundary;
		node->draw(window, canvas, box.center, &boundary);
		sf::Vector2f cursor(sf::Mouse::getPosition(*window));
		if (canvasNodes[i].contains(canvas.pixelPosToCanvas(cursor)))
			hoveredNode = i;
	}

	void _drawTree(sf::RenderWindow* window, const trees::Node* tree, size_t& i)
	{
		const auxillary::BoundingBox& box = canvasNodes[i].box;
		if (box.top <= canvas.view.bottom)
		{
			i += (tree->l == nullptr ? 0 : tree->l->n) + (tree->r == nullptr ? 0 : tree->r->n) + 1;
		}
		else if (box.right <= canvas.view.left)
		{
			i += (tree->l == nullptr ? 0 : tree->l->n) + 1;
			if (tree->r != nullptr)
			{
				auxillary::BoundingBox lineBox = auxillary::BoundingBox::CreateFromPoints(
					box.center, canvasNodes[i].box.center
				);
				if (canvas.view.overlaps(lineBox))
				{
					sf::RectangleShape line = canvas.getLine(
						{ lineBox.left, lineBox.top }, { lineBox.right, lineBox.bottom },
						trees::Node::outlineThickness
					);
					line.setFillColor(sf::Color::Black);
					window->draw(line);
				}
				_drawTree(window, tree->r, i);
			}
		}
		else if (box.left >= canvas.view.right)
		{
			i += 1;
			if (tree->l != nullptr)
			{
				auxillary::BoundingBox lineBox = auxillary::BoundingBox::CreateFromPoints(
					box.center, canvasNodes[i].box.center
				);
				if (canvas.view.overlaps(lineBox))
				{
					sf::RectangleShape line = canvas.getLine(
						{ lineBox.right, lineBox.top }, { lineBox.left, lineBox.bottom },
						trees::Node::outlineThickness
					);
					line.setFillColor(sf::Color::Black);
					window->draw(line);
				}
				_drawTree(window, tree->l, i);
			}
			i += (tree->r == nullptr ? 0 : tree->r->n);
		}
		else
		{
			size_t srcI = i;
			i += 1;
			if (tree->l != nullptr)
			{
				auxillary::BoundingBox lineBox = auxillary::BoundingBox::CreateFromPoints(
					box.center, canvasNodes[i].box.center
				);
				if (canvas.view.overlaps(lineBox))
				{
					sf::RectangleShape line = canvas.getLine(
						{ lineBox.right, lineBox.top }, { lineBox.left, lineBox.bottom },
						trees::Node::outlineThickness
					);
					line.setFillColor(sf::Color::Black);
					window->draw(line);
				}
				_drawTree(window, tree->l, i);
			}
			if (tree->r != nullptr)
			{
				auxillary::BoundingBox lineBox = auxillary::BoundingBox::CreateFromPoints(
					box.center, canvasNodes[i].box.center
				);
				if (canvas.view.overlaps(lineBox))
				{
					sf::RectangleShape line = canvas.getLine(
						{ lineBox.left, lineBox.top }, { lineBox.right, lineBox.bottom },
						trees::Node::outlineThickness
					);
					line.setFillColor(sf::Color::Black);
					window->draw(line);
				}
				_drawTree(window, tree->r, i);
			}
			drawNode(window, tree, srcI);
		}
	}

	void drawTree(sf::RenderWindow* window)
	{
		if (buildNewTree) calculateTree(), buildNewTree = false;
		const trees::Node* tree = getCurrentTreeRoot();
		hoveredNode = -1;
		if (tree != nullptr)
		{
			size_t i = 0;
			_drawTree(window, getCurrentTreeRoot(), i);
		}
	}

	void setupGridLines()
	{
		horizontalLineA = sf::RectangleShape(sf::Vector2f((float)config::WINDOW_SIZE_X, gridlineThickness));
		horizontalLineA.setOrigin(0.f, gridlineThickness / 2);
		horizontalLineA.setFillColor(gridlineColor);
		horizontalLineB = sf::RectangleShape(sf::Vector2f((float)config::WINDOW_SIZE_X, gridlineThickness));
		horizontalLineB.setOrigin(0.f, gridlineThickness / 2);
		horizontalLineB.setFillColor(gridlineAltColor);
		verticalLineA = sf::RectangleShape(sf::Vector2f(gridlineThickness, (float)config::WINDOW_SIZE_Y));
		verticalLineA.setOrigin(gridlineThickness / 2, 0.f);
		verticalLineA.setFillColor(gridlineColor);
		verticalLineB = sf::RectangleShape(sf::Vector2f(gridlineThickness, (float)config::WINDOW_SIZE_Y));
		verticalLineB.setOrigin(gridlineThickness / 2, 0.f);
		verticalLineB.setFillColor(gridlineAltColor);
	}

	void drawGrid(sf::RenderWindow* window)
	{
		for (long long x = (long long)std::floor(canvas.view.left); x <= std::ceil(canvas.view.right); ++x)
		{
			if (x % 2 == 0)
			{
				verticalLineB.setPosition(
					canvas.canvasPosToPixel(auxillary::vec2((float)x, canvas.view.top))
				);
				window->draw(verticalLineB);
			}
			else
			{
				verticalLineA.setPosition(
					canvas.canvasPosToPixel(auxillary::vec2((float)x, canvas.view.top))
				);
				window->draw(verticalLineA);
			}
		}
		for (long long y = (long long)std::floor(canvas.view.bottom); y <= std::ceil(canvas.view.top); ++y)
		{
			if (y % 2 == 0)
			{
				horizontalLineB.setPosition(
					canvas.canvasPosToPixel(auxillary::vec2(canvas.view.left, (float)y))
				);
				window->draw(horizontalLineB);
			}
			else
			{
				horizontalLineA.setPosition(
					canvas.canvasPosToPixel(auxillary::vec2(canvas.view.left, (float)y))
				);
				window->draw(horizontalLineA);
			}
		}
	}

	void insertNode()
	{
		if (selectedTree == trees::Trees::AVL)
		{
			avl.insert(inputNodeValue);
		}
		else if (selectedTree == trees::Trees::RB)
		{
			rb.insert(inputNodeValue);
		}
		else if (selectedTree == trees::Trees::Treap)
		{
			if (inputNodePriorValue == -1)
				treap.insert(inputNodeValue);
			else
				treap.insert(inputNodeValue, inputNodePriorValue);
		}
		else if (selectedTree == trees::Trees::Splay)
		{
			splay.insert(inputNodeValue);
		}
		buildNewTree = true;
		inputNodeValue = 0, inputNodePriorValue = -1;
	}

	void insertRandomNodes()
	{
		if (selectedTree == trees::Trees::AVL)
			avl.insertRandom(inputNodesCountValue);
		else if (selectedTree == trees::Trees::RB)
			rb.insertRandom(inputNodesCountValue);
		else if (selectedTree == trees::Trees::Treap)
			treap.insertRandom(inputNodesCountValue);
		else if (selectedTree == trees::Trees::Splay)
			splay.insertRandom(inputNodesCountValue);
		buildNewTree = true;
		inputNodesCountValue = 0;
	}

	void eraseNode()
	{
		if (selectedTree == trees::Trees::AVL)
			avl.erase(canvasNodes[hoveredNode].node->elem);
		else if (selectedTree == trees::Trees::RB)
			rb.erase(canvasNodes[hoveredNode].node->elem);
		else if (selectedTree == trees::Trees::Treap)
			treap.erase(canvasNodes[hoveredNode].node->elem);
		else if (selectedTree == trees::Trees::Splay)
			splay.erase(canvasNodes[hoveredNode].node->elem);
		buildNewTree = true;
	}

	void handleWindowEvents(sf::Window* window)
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			handleEvent(window, std::move(event));
		}
	}

	void handleEvent(sf::Window* window, const sf::Event&& event)
	{
		if (event.type == sf::Event::Closed)
		{
			window->close();
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::F1)
				displaySettings ^= 1;
			else if (event.key.code == sf::Keyboard::F2)
				displayNodeActions ^= 1;
			else if (event.key.code == sf::Keyboard::F3)
				displayNodeInfo ^= 1;
			else if (event.key.code == sf::Keyboard::F4)
				displayCanvasInfo ^= 1;
			else if (event.key.code == sf::Keyboard::Escape && displayInsertNode)
				closeInsertNodePopup(true);
			else if (event.key.code == sf::Keyboard::Escape && displayInsertRandNodes)
				closeInsertRandomNodesPopup(true);
			else if (event.key.code == sf::Keyboard::Enter && displayInsertNode && canInsertNode)
				closeInsertNodePopup();
			else if (event.key.code == sf::Keyboard::Enter && displayInsertRandNodes && canInsertRandNodes)
				closeInsertRandomNodesPopup();
		}
		else if (!ImGui::GetIO().WantCaptureMouse)
		{
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (hoveredNode == -1)
				{
					movingCanvas = true;
					savedCursor = sf::Vector2f((float)event.mouseButton.x, (float)event.mouseButton.y);
				}
				else
				{
					eraseNode();
				}
			}
			else if (event.type == sf::Event::MouseMoved && movingCanvas)
			{
				sf::Vector2f cursor((float)event.mouseMove.x, (float)event.mouseMove.y);
				canvas.topLeft += canvas.pixelVecToCanvas(savedCursor - cursor).Yconjugate();
				canvas.calculateView();
				savedCursor = cursor;
			}
			else if (event.type == sf::Event::MouseButtonReleased && movingCanvas)
			{
				sf::Vector2f cursor((float)event.mouseButton.x, (float)event.mouseButton.y);
				canvas.topLeft += canvas.pixelVecToCanvas(savedCursor - cursor).Yconjugate();
				canvas.calculateView();
				movingCanvas = false;
			}
			else if (event.type == sf::Event::MouseWheelScrolled)
			{
				sf::Vector2f cursor((float)event.mouseWheelScroll.x, (float)event.mouseWheelScroll.y);
				auxillary::vec2 oldCursor = canvas.pixelPosToCanvas(cursor);
				canvas.width += -event.mouseWheelScroll.delta * scc::Canvas::wheelStrength;
				canvas.width = std::max(
					scc::Canvas::minWidth, std::min(canvas.width, scc::Canvas::maxWidth)
				);
				canvas.topLeft += oldCursor - canvas.pixelPosToCanvas(cursor);
				canvas.calculateView();
			}
		}
	}

	void showInsertNodePopup()
	{
		canInsertNode = (inputNodeValue > -1 && inputNodePriorValue > -2);
		ImGui::OpenPopup("Insert node");
		if (ImGui::BeginPopupModal("Insert node", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Node value:");
			if (ImGui::IsWindowAppearing())
				ImGui::SetKeyboardFocusHere();
			ImGui::InputInt("##InputNodeValue", &inputNodeValue);
			if (selectedTree == trees::Trees::Treap)
			{
				ImGui::Dummy({ 0., 1. });
				ImGui::Text("Priority value:");
				ImGui::InputInt("##InputPriorityValue", &inputNodePriorValue);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Enter '-1' for random value");
			}
			ImGui::Dummy({ 0., 5. });
			ImGui::Dummy({ 0., 0. });
			ImGui::SameLine(ImGui::GetWindowWidth() - 100.5f);
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::Button("Abort"))
				closeInsertNodePopup(true);
			ImGui::SameLine(ImGui::GetWindowWidth() - 50.5f);
			ImGui::SetNextItemWidth(50.f);
			ImGui::BeginDisabled(!canInsertNode);
			if (ImGui::Button("Apply"))
				closeInsertNodePopup();
			ImGui::EndDisabled();
			ImGui::EndPopup();
		}
	}

	void closeInsertNodePopup(bool abort)
	{
		if (abort)
		{
			inputNodeValue = 0, inputNodePriorValue = -1, displayInsertNode = false;
			return;
		}
		insertNode();
		displayInsertNode = false;
	}

	void showInsertRandomNodesPopup()
	{
		canInsertRandNodes = (inputNodesCountValue > -1);
		ImGui::OpenPopup("Insert random nodes");
		if (ImGui::BeginPopupModal("Insert random nodes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Nodes amount:");
			if (ImGui::IsWindowAppearing())
				ImGui::SetKeyboardFocusHere();
			ImGui::InputInt("##InputNodesCount", &inputNodesCountValue);
			ImGui::Dummy({ 0., 5. });
			ImGui::Dummy({ 0., 0. });
			ImGui::SameLine(ImGui::GetWindowWidth() - 100.5f);
			ImGui::SetNextItemWidth(50.f);
			if (ImGui::Button("Abort"))
				closeInsertRandomNodesPopup(true);
			ImGui::SameLine(ImGui::GetWindowWidth() - 50.5f);
			ImGui::SetNextItemWidth(50.f);
			ImGui::BeginDisabled(!canInsertRandNodes);
			if (ImGui::Button("Apply"))
				closeInsertRandomNodesPopup();
			ImGui::EndDisabled();
			ImGui::EndPopup();
		}
	}

	void closeInsertRandomNodesPopup(bool abort)
	{
		if (abort)
		{
			inputNodesCountValue = 1, displayInsertRandNodes = false;
			return;
		}
		insertRandomNodes();
		displayInsertRandNodes = false;
	}

	void showSettingsWindow()
	{
		ImGui::Begin("Settings");
		ImGui::Text("Tree:");
		if (ImGui::BeginCombo("##TreeSelect", trees::treeToString(selectedTree)))
		{
			for (auto tree : trees::TreesIter)
			{
				if (ImGui::Selectable(trees::treeToString(tree), tree == selectedTree))
				{
					if (tree != selectedTree)
					{
						selectedTree = tree;
						canvas.restoreDefaultView();
						buildNewTree = true;
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Dummy({ 0., 3. });
		ImGui::Checkbox("Show grids", &showGrids);
		ImGui::Dummy({ 0., 1. });
		ImGui::Text("Node spacing:");
		if (ImGui::SliderInt("##NodeSpacingSlider", &nodeSpacing, 10, 100, "%d%%"))
		{
			trees::Node::spacing = (float)nodeSpacing / 100 * trees::Node::diameter;
			buildNewTree = true;
		}
		ImGui::End();
	}

	void showNodeInfoWindow()
	{
		ImGui::Begin("Node info");
		if (hoveredNode == -1)
		{
			ImGui::TextWrapped("Select a node to see information about it.");
		}
		else
		{
			ImGui::Text("Height: %llu", canvasNodes[hoveredNode].node->h);
			ImGui::Text("Nodes count: %llu", canvasNodes[hoveredNode].node->n);
			ImGui::Text("Node value: %llu", canvasNodes[hoveredNode].node->elem);
			ImGui::BeginDisabled(selectedTree != trees::Trees::Treap);
			ImGui::Text(
				selectedTree != trees::Trees::Treap ? "Node priority: None" : "Node priority: %llu",
				((trees::Treap::NodeType*)(canvasNodes[hoveredNode].node))->prior
			);
			ImGui::EndDisabled();
			ImGui::BeginDisabled(selectedTree != trees::Trees::RB);
			ImGui::Text(
				selectedTree != trees::Trees::RB ? "Node color: None" : "Node color: %s",
				((trees::RBTree::NodeType*)(canvasNodes[hoveredNode].node))->red ? "Red" : "Black"
			);
			ImGui::EndDisabled();
		}
		ImGui::End();
	}

	void showNodeActionsWindow()
	{
		ImGui::Begin("Node actions");
		if (ImGui::Button("Insert Node"))
			displayInsertNode = true;
		ImGui::SameLine();
		if (ImGui::Button("Insert Random"))
			displayInsertRandNodes = true;
		ImGui::End();
	}

	void showCanvasInfoWindow(sf::Window* window)
	{
		sf::Vector2f cursor(sf::Mouse::getPosition(*window));
		auxillary::vec2 canvasCursor = canvas.pixelPosToCanvas(cursor);
		bool outsideWindow = (
			!window->hasFocus() || cursor.x < 0 || cursor.x > config::WINDOW_SIZE_X || 
			cursor.y < 0 || cursor.y > config::WINDOW_SIZE_Y
		);
		ImGui::Begin("Canvas info");
		ImGui::BeginDisabled(outsideWindow);
		ImGui::SameLine(10.f);
		ImGui::SetNextItemWidth(95.f);
		ImGui::Text(outsideWindow ? "x: None" : "x: %f", canvasCursor.x);
		ImGui::SameLine(110.f);
		ImGui::SetNextItemWidth(95.f);
		ImGui::Text(outsideWindow ? "y: None" : "y: %f", canvasCursor.y);
		ImGui::Dummy({ 0., 3. });
		ImGui::Dummy({ 0., 0. });
		ImGui::SameLine(50.f);
		if (ImGui::Button("Reset View", { 110.f, 20.f }))
			canvas.restoreDefaultView();
		ImGui::EndDisabled();
		ImGui::End();
	}
}
