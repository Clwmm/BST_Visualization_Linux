// Mateusz Ka³wa

#ifndef VISUAL_BINARY_TREE
#define VISUAL_BINARY_TREE

#include "SFML/Graphics.hpp"
#include <string>
#include <iostream>
#include <cmath>

namespace vbt
{
	constexpr float CIRCLE_SIZE = 23.f;
	constexpr float OFFSET_X_MULTI = 15.f;
	constexpr float OFFSET_Y = 80.f;
	constexpr float MOVE_SPEED = 200.f;
	constexpr float ANIMATION_SPEED = 0.5f;

	enum class Status
	{
		None, Insert, Search, Delete
	};

	enum class Side
	{
		Left, Right, None
	};

	template <class T>
	class VisualBinaryNode
	{
	public:
		T key;
		VisualBinaryNode* right = nullptr;
		VisualBinaryNode* left = nullptr;
		VisualBinaryNode* parent = nullptr;

		sf::CircleShape circle;
		sf::Vector2f position;
		sf::Vector2f newPosition;
		bool repositioning = false;
		sf::Text text;

		Status status = Status::None;

		template <class U>
		VisualBinaryNode(U&& key, sf::Vector2f position, sf::Font& font)
		{
			this->key = std::forward<U>(key);
			this->position = position;

			std::string strText = std::to_string(this->key);
			if (strText.size() < 2)
				strText = "0" + strText;
			text.setFont(font);
			text.setCharacterSize(24);
			text.setString(strText);
			text.setPosition(sf::Vector2f(position.x - 13, position.y - 15));

			circle.setPointCount(60);
		}

		void update(const float& deltaTime)
		{
			if (this->repositioning)
			{
				sf::Vector2f dir = newPosition - position;
				float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

				if (dist > 1.0f)
				{
					dir /= dist;
					position.x += dir.x * MOVE_SPEED * deltaTime;
					position.y += dir.y * MOVE_SPEED * deltaTime;
				}
				else
					this->repositioning = false;
			}
		}

		void draw(sf::RenderWindow& window)
		{
			circle.setPosition(position);

			switch (this->status)
			{
			case Status::None:
				circle.setFillColor(sf::Color::White);
				break;
			case Status::Delete:
				circle.setFillColor(sf::Color::Red);
				break;
			case Status::Search:
				circle.setFillColor(sf::Color::Yellow);
				break;
			case Status::Insert:
				circle.setFillColor(sf::Color::Green);
				break;
			}

			circle.setRadius(CIRCLE_SIZE);
			circle.setOrigin(CIRCLE_SIZE, CIRCLE_SIZE);
			window.draw(circle);
			

			circle.setPosition(position);
			circle.setFillColor(sf::Color::Black);
			circle.setRadius(CIRCLE_SIZE - 3.f);
			circle.setOrigin(CIRCLE_SIZE - 3.f, CIRCLE_SIZE - 3.f);
			window.draw(circle);

			std::string strText = std::to_string(this->key);
			if (strText.size() < 2)
				strText = "0" + strText;
			text.setString(strText);
			text.setPosition(sf::Vector2f(position.x - 13, position.y - 15));
			window.draw(text);
		}
	};

	template <class T>
	class VisualBinaryTree
	{
	private:
		using node_ptr = VisualBinaryNode<T>*;
		node_ptr root = nullptr;

		sf::Vertex line[2];
		sf::Font font;

		int size = 0;

		struct UpdateStatus
		{
			Status status = Status::None;
			Side side = Side::None;
			node_ptr actual_node = nullptr;
			node_ptr previous_node = nullptr;
			node_ptr delete_node = nullptr;
			sf::Vector2f position;
			T x;
			float time = 0.f;

			void clear()
			{
				if (this->actual_node)
					this->actual_node->status = Status::None;
				if (this->previous_node)
					this->previous_node->status = Status::None;
				this->status = Status::None;
				this->actual_node = nullptr;
				this->previous_node = nullptr;
				this->time = 0.f;
				this->side = Side::None;
			}
		};

		UpdateStatus status;

		void repositioning(const int& depth, node_ptr node)
		{
			float offsetX = static_cast<float>(std::pow(static_cast<float>(depth), 2)) * OFFSET_X_MULTI;
			float offsetY = OFFSET_Y;

			if (node == nullptr)
				return;

			if (node->left)
			{
				sf::Vector2f newPosition(node->position.x - offsetX, node->position.y + offsetY);
				node->left->newPosition = newPosition;
				node->left->repositioning = true;
				this->repositioning(depth - 1, node->left);
			}
			if (node->right)
			{
				sf::Vector2f newPosition(node->position.x + offsetX, node->position.y + offsetY);
				node->right->newPosition = newPosition;
				node->right->repositioning = true;
				this->repositioning(depth - 1, node->right);
			}
		}

		void insert_right_visual(const T& key, node_ptr node, node_ptr parent)
		{
			node = new VisualBinaryNode<T>(key, sf::Vector2f(parent->position.x, parent->position.y), this->font);
			parent->right = node;
			node->parent = parent;
			return;
		}

		void insert_left_visual(const T& key, node_ptr node, node_ptr parent)
		{
			node = new VisualBinaryNode<T>(key, sf::Vector2f(parent->position.x, parent->position.y), this->font);
			parent->left = node;
			node->parent = parent;
			return;
		}

		void inorder_(node_ptr node, std::string& text)
		{
			if (node != nullptr)
			{
				this->inorder_(node->left, text);
				text += std::to_string(node->key) + " ";
				this->inorder_(node->right, text);
			}
		}

		void preorder_(node_ptr node, std::string& text)
		{
			if (node != nullptr)
			{
				text += std::to_string(node->key) + " ";
				this->inorder_(node->left, text);
				this->inorder_(node->right, text);
			}
		}

		void postorder_(node_ptr node, std::string& text)
		{
			if (node != nullptr)
			{
				this->inorder_(node->left, text);
				this->inorder_(node->right, text);
				text += std::to_string(node->key) + " ";
			}
		}

		void passingTheNodes(node_ptr node, T& key, std::string& alert)
		{
			if (status.previous_node)
				status.previous_node->status = Status::None;

			switch (status.status)
			{
			case Status::Search:
				if (node != nullptr)
					node->status = Status::Search;
				else
				{
					status.status = Status::None;
					alert = "Not found: " + std::to_string(key);
					return;
				}
				if (node->key == key)
				{
					status.status = Status::None;
					alert = "Found: " + std::to_string(key);
					return;
				}
				break;

			case Status::Delete:
				if (node != nullptr)
					node->status = Status::Delete;
				else
				{
					status.status = Status::None;
					alert = "Not found: " + std::to_string(key);
					return;
				}
				if (node->key == key)
				{
					status.status = Status::None;
					status.delete_node = node;
					return;

					this->remove_visual(node);
					alert = "Deleted: " + std::to_string(key);
					--size;
					return;
				}
				break;

			case Status::Insert:
				if (node == nullptr)
				{
					if (root == nullptr)
						root = new VisualBinaryNode<T>(key, status.position, this->font);
					else
					{
						switch (status.side)
						{
						case Side::Left:
							this->insert_left_visual(key, node, status.previous_node);
							break;
						case Side::Right:
							this->insert_right_visual(key, node, status.previous_node);
							break;
						default:
							break;
						}
					}
					status.status = Status::None;
					alert = "Added: " + std::to_string(key);
					++size;
					return;
				}
				else
					node->status = Status::Insert;
				break;
			default:
				break;
			}


			status.previous_node = node;

			if (key < node->key)
			{
				status.side = Side::Left;
				status.actual_node = node->left;
			}
			else
			{
				status.side = Side::Right;
				status.actual_node = node->right;
			}
				
		}

		void remove_visual(node_ptr node)
		{
			// ONLY ONE CHILD EXISTS
			if (node->left == nullptr && node->right != nullptr)
			{
				node_ptr right = node->right;
				if (node == root)
				{
					right->parent = nullptr;
					root = right;
					delete node;
					return;
				}
				right->parent = node->parent;
				if (node->parent->right == node)
					node->parent->right = right;
				else
					node->parent->left = right;
				delete node;
				return;
			}
			else if (node->right == nullptr && node->left != nullptr)
			{
				node_ptr left = node->left;
				if (node == root)
				{
					left->parent = nullptr;
					root = left;
					delete node;
					return;
				}
				left->parent = node->parent;
				if (node->parent->right == node)
					node->parent->right = left;
				else
					node->parent->left = left;
				delete node;
				return;
			}
			// NODE IS A LEAF
			else if (node->right == nullptr && node->left == nullptr)
			{
				if (node == root)
				{
					delete node;
					root = nullptr;
					return;
				}
				if (node->parent->right == node)
					node->parent->right = nullptr;
				else
					node->parent->left = nullptr;
				delete node;
				return;
			}
			// NODE HAVE BOTH CHILDS
			else
			{
				Iterator it(node);
				++it;
				node_ptr successor = *it;

				if (successor == node->right)
				{
					node->key = successor->key;
					node->position = successor->position;
					if (successor->right)
					{
						node->right = successor->right;
						node->right->parent = node;
					}
					else
						node->right = nullptr;
					delete successor;
					return;
				}

				if (successor->right == nullptr)
				{
					node->key = successor->key;
					node->position = successor->position;
					successor->parent->left = nullptr;
					delete successor;
					return;
				}
				else
				{
					node->key = successor->key;
					node->position = successor->position;
					successor->parent->left = successor->right;
					successor->right->parent = successor->parent;
					delete successor;
					return;
				}
			}
		}

		void clear_(node_ptr node)
		{
			if (node != nullptr)
			{
				clear_(node->left);
				clear_(node->right);
				delete node;
			}
		}

		void draw_(sf::RenderWindow& window, node_ptr node)
		{
			if (node == nullptr)
				return;

			if (node->right != nullptr)
			{
				line[0] = node->position;
				line[1] = node->right->position;
				window.draw(line, 2, sf::Lines);
				this->draw_(window, node->right);
			}
			if (node->left != nullptr)
			{
				line[0] = node->position;
				line[1] = node->left->position;
				window.draw(line, 2, sf::Lines);
				this->draw_(window, node->left);
			}

			node->draw(window);
		}

		int depth_(node_ptr node)
		{
			if (node == nullptr)
				return 0;
			int leftDepth = this->depth_(node->left);
			int rightDepth = this->depth_(node->right);
			return std::max(leftDepth, rightDepth) + 1;
		}

	public:

		VisualBinaryTree(sf::Font& font)
		{
			this->font = font;
		}

		struct Iterator
		{
			node_ptr current;
			Iterator(node_ptr node)
				: current(node) {}

			node_ptr operator*() { return current; }
			Iterator& operator++()
			{
				if (current->right)
				{
					current = current->right;
					if (current->left)
						while (current->left)
							current = current->left;
				}
				else
				{
					node_ptr parent = current->parent;
					while (parent != nullptr)
					{
						if (current != parent->right)
							break;
						current = parent;
						parent = parent->parent;
					}
					current = parent;
				}
				return *this;
			}

			bool operator==(const Iterator& other) const
			{
				return this->current == other.current;
			}

			bool operator!=(const Iterator& other) const
			{
				return this->current != other.current;
			}
		};

		void insert(const T& x, const sf::Vector2f& position = sf::Vector2f(0.f, 0.f))
		{
			status.clear();
			status.status = Status::Insert;
			status.actual_node = root;
			status.x = x;
			status.position = position;
		}

		int depth() { return this->depth_(root); }

		std::string inorder()
		{
			std::string text;
			this->inorder_(root, text);
			return text;
		}

		std::string preorder()
		{
			std::string text;
			this->preorder_(root, text);
			return text;
		}

		std::string postorder()
		{
			std::string text;
			this->postorder_(root, text);
			return text;
		}

		void update(const float& deltatime, std::string& alert)
		{
			switch (status.status)
			{
			case Status::None:
				if (status.time <= 0.f)
				{
					if (status.delete_node)
					{
						alert = "Deleted: " + std::to_string(status.delete_node->key);
						this->remove_visual(status.delete_node);
						status.delete_node = nullptr;
						--size;
					}
					status.clear();
				}
					
				else
					status.time -= deltatime;
				break;

			default:
				if (status.time <= 0.f)
				{
					this->passingTheNodes(status.actual_node, status.x, alert);
					status.time = ANIMATION_SPEED;
				}
				else
					status.time -= deltatime;
				break;
				break;
			}

			this->repositioning(this->depth(), this->root);
			for (Iterator it = this->begin(); it != this->end(); ++it)
			{
				node_ptr temp = *it;
				temp->update(deltatime);
			}
		}

		void search(const T& key)
		{
			status.clear();
			status.status = Status::Search;
			status.actual_node = root;
			status.x = key;
		}

		T minimum()
		{
			if (root == nullptr)
				return 0;
			node_ptr temp = root;
			while (temp->left)
				temp = temp->left;
			return temp->key;
		}

		T maximum()
		{
			if (root == nullptr)
				return 0;
			node_ptr temp = root;
			while (temp->right)
				temp = temp->right;
			return temp->key;
		}

		void remove(const T& key)
		{
			status.clear();
			status.status = Status::Delete;
			status.actual_node = root;
			status.x = key;
			return;
		}

		void clear()
		{
			this->clear_(root);
			root = nullptr;
			this->size = 0;
		}

		int getSize()
		{
			return this->size;
		}

		float getWidth()
		{
			float sum = 0;
			for (int n = this->depth(); n >= 1; --n)
				sum += std::pow(static_cast<float>(n), 2.f) * OFFSET_X_MULTI;
			return sum * 2;
		}

		float getHeight()
		{
			return static_cast<float>(this->depth()) * OFFSET_Y;
		}

		node_ptr getRoot() { return this->root; }

		void draw(sf::RenderWindow& window) { this->draw_(window, root); }

		Iterator begin()
		{
			node_ptr temp = root;
			if (temp)
				while (temp->left)
					temp = temp->left;
			return Iterator(temp);
		}
		Iterator end() { return Iterator(nullptr); }
	};

};


#endif // !VISUAL_BINARY_TREE