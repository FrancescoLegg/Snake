#include <SFML/Graphics.hpp>
#include <iostream>

class Snake {
public:
    Snake(float size, float stepTime)
        : direction(1.f, 0.f), stepTime(stepTime), elapsedTime(0.f), cellSize(size)
    {
        sf::RectangleShape segment(sf::Vector2f(size, size));
        segment.setFillColor(sf::Color::White);
        segment.setPosition(0.f, 0.f);
        body.push_back(segment);
    }

    void snakeMoves() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && direction.x == 0)
            direction = {1.f, 0.f};
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && direction.x == 0)
            direction = {-1.f, 0.f};
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && direction.y == 0)
            direction = {0.f, -1.f};
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && direction.y == 0)
            direction = {0.f, 1.f};
    }

    void wrapAround(sf::Vector2u windowSize, float cellSize){
        sf::Vector2f pos = body[0].getPosition();

        if(pos.x < 0){
            pos.x = windowSize.x - cellSize;
        }else if(pos.x > windowSize.x){
            pos.x = 0;
        }

        if(pos.y < 0){
            pos.y = windowSize.y - cellSize;
        }else if(pos.y > windowSize.y){
            pos.y = 0;
        }

        body[0].setPosition(pos);
    }

    bool checkSelfCollision() {
        //if (body.size() < 3) return false; // troppo corto per collidere

        sf::FloatRect headBounds = body[0].getGlobalBounds();

        // inizia dal 1, ma fermati PRIMA dell’ultimo segmento
        for (size_t i = 1; i < body.size() - 1; i++) {
            if (body[i].getGlobalBounds().intersects(headBounds)) {
                return true;
            }
        }

        return false;
    }


    void update(float deltaTime, sf::Vector2u windowSize, float cellSize) {
        elapsedTime += deltaTime;
        if (elapsedTime >= stepTime) {
            // sposta i segmenti (coda -> testa)
            for (int i = body.size() - 1; i > 0; --i) {
                body[i].setPosition(body[i - 1].getPosition());
            }

            // muovi la testa
            body[0].move(direction * cellSize);

            // effetto Pac-Man
            wrapAround(windowSize, cellSize);

            elapsedTime = 0.f;
    }
}


    void grow() {
        sf::RectangleShape newSegment(sf::Vector2f(cellSize, cellSize));
        newSegment.setFillColor(sf::Color::White);

        // Metti il nuovo segmento nella stessa posizione DELL'ULTIMO PEZZO,
        // ma aggiungilo solo dopo che si sarà mosso nel prossimo update
        sf::Vector2f lastPos = body.back().getPosition();
        newSegment.setPosition(lastPos);

        body.push_back(newSegment);
    }

    void draw(sf::RenderWindow& window) {
        for (auto& segment : body)
            window.draw(segment);
    }

    sf::FloatRect getHeadBounds() const {
        return body.front().getGlobalBounds();
    }

private:
    std::vector<sf::RectangleShape> body;
    sf::Vector2f direction;
    float stepTime;
    float elapsedTime;
    float cellSize;

    void move() {
        // Sposta ogni segmento nella posizione di quello precedente (da coda → testa)
        for (int i = body.size() - 1; i > 0; --i) {
            body[i].setPosition(body[i - 1].getPosition());
        }

        // Muove la testa nella nuova direzione
        body[0].move(direction * cellSize);
    }
};


class Apple{
    public:
        Apple(float size, sf::Vector2f position){
            body.setSize(sf::Vector2f(size, size));
            body.setFillColor(sf::Color::Red);
            body.setPosition(position);
        }

        void respawn(sf::Vector2u windowSize, float cellSize){
            int cellasX = windowSize.x / cellSize;
            int cellasY = windowSize.y / cellSize;

            int randomX = rand() % cellasX;
            int randomY = rand() % cellasY;

            float x = randomX * cellSize;
            float y = randomY * cellSize;

            body.setPosition(x,y);
        }

        void draw(sf::RenderWindow &window){
            window.draw(body);
        }
        sf::FloatRect getBounds() const {
            return body.getGlobalBounds();
        }

    private:
        sf::RectangleShape body;
};


int main(){ 
    
    sf::RenderWindow window(sf::VideoMode(1920,1080), "Snake");
    window.setFramerateLimit(120);

    //Istanza Snake
    Snake snake(50.f, 0.2f);
    Apple apple(50.f, {500.f, 500.f});


    int snakeScore = 0;

    sf::Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    sf::Text scoreText("0", font, 40);
    scoreText.setFillColor(sf::Color::White);
    // ottieni la dimensione del testo
    sf::FloatRect textRect = scoreText.getLocalBounds();
    // posizione centrale orizzontale
    scoreText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    scoreText.setPosition(1920 / 2.0f, 20); // 1920 = larghezza finestra, 20 = distanza dall’alto

    //Clock
    sf::Clock clock;

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed)
                window.close();
        }


        float deltaTime = clock.restart().asSeconds();

        snake.snakeMoves();
        snake.update(deltaTime, window.getSize(), 50.f);

        window.clear(sf::Color::Black);

        snake.draw(window);
        apple.draw(window);

        // Prima controlla collisione con sé stesso
        if (snake.checkSelfCollision()) {
            std::cout << "Hai perso! Collisione con il corpo.\n";
            window.close();
        }

        if (snake.getHeadBounds().intersects(apple.getBounds())) {
            apple.respawn(window.getSize(), 50.f);
            snake.grow();
            snakeScore += 1;
        }

        scoreText.setString("Score: " + std::to_string(snakeScore));
        window.draw(scoreText);

        window.display();


    }

   return 0;

}






/*
g++ main.cpp -o snake -lsfml-graphics -lsfml-window -lsfml-system 
./snake
*/