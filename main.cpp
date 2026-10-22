#include <iostream>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <chrono>
#include <math.h>

/////////
#include <string>
#include <source_location> // https://en.cppreference.com/w/cpp/utility/source_location
#include <filesystem>
    
//For slightly enhanced debugging
std::ostream& dump_location(std::source_location loc = std::source_location::current())
{
    return std::cout << std::filesystem::path( loc.file_name() ).filename() << ", function: "
                     << loc.function_name() << ", line: " << loc.line() << '\n' ;
}
///////////////////////////////////
/*
 clock_t newTime = clock(), duration = newTime - clockTimer;
        clockTimer = newTime;
        std::cout << "CPU Time (Thread Aggregate): " << (float)(duration) / CLOCKS_PER_SEC * 1000 << "ms" << '\n';

 const auto t_end = std::chrono::high_resolution_clock::now();
 std::cout << "CLOCK/Real Time:          " << std::chrono::duration<double, std::milli>(t_end - t_start) << '\n';
 t_start = t_end;
*/


//Functions for speed optimization
namespace Timer{

static std::chrono::time_point<std::chrono::high_resolution_clock> chronoTime;
static inline std::source_location beginLocation = std::source_location::current();

//call this function to reset the (wall clock) timer and record the line it's on
void resetTimer(std::source_location loc = std::source_location::current()){
   beginLocation = std::forward<std::source_location>(loc);
   chronoTime =  std::chrono::high_resolution_clock::now();
}

//call this function to get the duration between now and when the timer was reset. Outputs start and end lines, and the duration
void getDuration(std::source_location loc = std::source_location::current()){
    std::cout << "Duration from Lines: (start) " <<  beginLocation.line() << " to (end) " << loc.line() << " -- "; 
    std::cout << std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - chronoTime) << '\n';
}

}

//


class CI{//ColorInts
public:
constexpr static std::uint32_t intMax = std::numeric_limits<std::uint32_t>::max();

static std::uint32_t colorToInt(const sf::Color& color){
    return (color.r << 24) + (color.g << 16) + (color.b << 8) + color.a - 1;//0,0,0,0 is invalid unless otherwise specified//this fixes blank spaces but keep in mind White will wrap around to 0,0,0,0.
}

static sf::Color intToColor(std::uint32_t input){
       ++input;//this is to ensure that an alpha of 0 is never selected. If you don't do this, empty space will map to an index of 0.
       return sf::Color((input >> 24), (input >> 16), (input >> 8), input);
}
};


int main()
{ 
    
    std::string windowName = "App";
    bool fullscreenEnabled = 1;//yes I know this could be wrong if I change the window params
    sf::Vector2u windowResolution{1920,1080};
    sf::RenderWindow window(sf::VideoMode({100,100}), windowName,sf::State::Windowed);
    
    if (fullscreenEnabled)
    window.create(sf::VideoMode(windowResolution), windowName,sf::State::Fullscreen);
        else
    window.create(sf::VideoMode(windowResolution), windowName,sf::State::Windowed);


    window.setFramerateLimit(63);
    window.setVerticalSyncEnabled(1);


    sf::Color color = CI::intToColor(9000);

    std::cout << CI::colorToInt(color) << std::endl;

    sf::RenderTexture rendTex;
    if (rendTex.resize(window.getSize()));
    rendTex.clear(sf::Color(0,0,0,0));

    constexpr unsigned int seglength = 2;
    constexpr unsigned int boxCount = 500*seglength;
    
    unsigned int boxes[boxCount];

    for (unsigned int i = 0; i<boxCount; i+=seglength){
        boxes[i] = (rand()%window.getSize().x);
        boxes[i+1] = (rand()%window.getSize().y);
}

    sf::Texture boxTexture{sf::Image({128,128},sf::Color::White)};
    sf::Sprite boxSprite{boxTexture};

      for (unsigned int i = 0; i<boxCount; i+=seglength){
        boxSprite.setPosition({(float)boxes[i],(float)boxes[i+1]});
        boxSprite.setColor(CI::intToColor(i/seglength));
        rendTex.draw(boxSprite,sf::BlendNone);
        }
        rendTex.display();
        
    sf::Sprite outSprite{rendTex.getTexture()};
        
    Timer::resetTimer();
    sf::Image outImg{rendTex.getTexture().copyToImage()};//this takes 8-16ms. Do not do this dynamically. Try using a compute shader
    Timer::getDuration();
   

    while (window.isOpen())
    {
         
        if (window.hasFocus()){
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()||sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
                window.close();
                
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab)){

                fullscreenEnabled = !fullscreenEnabled;

                if (fullscreenEnabled) 
                    window.create(sf::VideoMode(window.getSize()), windowName,sf::State::Fullscreen);
                    else
                    window.create(sf::VideoMode(window.getSize()), windowName,sf::Style::Default,sf::State::Windowed);
            }

        }

window.clear(sf::Color(100,100,100));//this handles timing for the window it seems, not display()
 //Timer::resetTimer();

        window.draw(outSprite);

        //Timer::resetTimer();
        std::uint32_t index = CI::intMax;
        {
        const sf::Vector2u mousePos = (sf::Vector2u)sf::Mouse::getPosition(window);
        if (mousePos.x<outImg.getSize().x && mousePos.y<outImg.getSize().y)
        index = CI::colorToInt(outImg.getPixel(mousePos));
        }
        
        
        if (index!=CI::intMax){       

        boxSprite.setPosition({(float)boxes[index*seglength],(float)boxes[index*seglength+1]});
        boxSprite.setColor(sf::Color::Red);
        window.draw(boxSprite);
}
//Timer::getDuration();
        //do drawing here


        window.display();
       // Timer::getDuration();
    }
}

}

