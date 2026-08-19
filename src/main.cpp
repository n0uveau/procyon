#include <SFML/Graphics.hpp>
#include <optional>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1280u, 720u }), "Procyon");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                window.setView(
                    sf::View(sf::FloatRect({ 0.f, 0.f }, sf::Vector2f(resized->size))));
        }

        window.clear(sf::Color(18, 18, 24));
        window.display();
    }
}