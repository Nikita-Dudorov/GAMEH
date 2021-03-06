#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <unistd.h>
#include <vector>
#include "player.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Box2D/Box2D.h"
#include "object.h"
#include "MyContactListener.h"
#include "virtualPlayer.h"
#include "weapon.h"
#include "handWeapon.h"
#include "grenade.h"
#include "Map/level.h"
#include "GameCore.h"

void addGid(){

    std::ifstream inFile;
    std::ofstream outFile;
    std::string s;
    outFile.open("/Map/TestEdited.tmx");
    inFile.open("/Map/Test2.tmx");
    while (getline(inFile,s)){
        if (s.find("<tile")!=-1){
            if(s.find("gid")==-1){
                s =  "<tile gid=\"0\"/>";
            }
        }
        s +="\n";
        outFile<<s;

    }
    inFile.close();
    outFile.close();
}


inline std::pair<float ,float > getMedian(std::vector<player>& players){
    sf::Vector2f m;
    for (int i = 0; i <players.size(); ++i) {
        m+=players[i].bound.getPosition();
    }

    std::pair<float ,float > temp(m.x/2,m.y/2);
    return temp;
}
//in process...
inline sf::Vector2f zoom(player const&PlayerA, player const &PlayerB,sf::RenderWindow &window){
    sf::Vector2f c = PlayerB.bound.getPosition()-PlayerA.bound.getPosition();//proportions
    float sizeWin = window.getSize().x*window.getSize().x+window.getSize().y*window.getSize().y;
    if(c.x<0)c.x = -c.x;
    float k =(float)window.getSize().y/(float)window.getSize().x;
    if(c.y<0)c.y = -c.y;


    float d = c.x*c.x+c.y*c.y;//think about function
    int frac = 3;
    float capacity = 2.0f;
    if(sqrt(d)<sqrt(sizeWin)/frac){
        return sf::Vector2f(capacity*sqrt(sizeWin/(1+k*k))/frac,capacity*(sqrt(sizeWin/(1+k*k))/frac)*k);
    } else{


        return sf::Vector2f(capacity*(sqrt(d/(1+k*k))),capacity*(sqrt(d/(1+k*k)))*k);

    }
}


int main() {

#if 0

    int x;
    std::cin>>x;
    if(x==1){

        if (socket[0].bind(54000) != sf::Socket::Done)
        {
            exit(0);
        }

    } else{

        if (socket[0].bind(54001) != sf::Socket::Done)
        {
            exit(0);
        }
    }

#endif
    MyContactListener myContactListenerInstance;
    b2Vec2 gravity(0.0f, -15.81f);
    b2World world(gravity);
    world.SetContactListener(&myContactListenerInstance);

    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

    window.setFramerateLimit(60);

    sf::View view1;
    view1.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    view1.setCenter(sf::Vector2f(view1.getSize().x / 2, view1.getSize().y / 2));
    window.setView(view1);




    GameCore game;
    game.initLvl(world);
    game.loadTextures();
    game.loadSoundBuffers();
    game.createMovableObjects(window,world);



    float32 timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    std::pair<float ,float>median;


    std::vector<sf::UdpSocket> socket(2);
    socket[0].setBlocking(false);
    socket[1].setBlocking(false);
    int x = 1;

    sf::Music music;
    if (!music.openFromFile("C:/Box2D-master/game_includes/Game-master/SFX/Megalovania.ogg"))
    {
    	printf("failure when loading music\n");
        return 1;
    }
    music.setVolume(22.f);
    music.play();

    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {


            if (event.type == sf::Event::Closed)
                window.close();
            for (int i = 0; i <game.players.size(); ++i) {
                game.players[i].checkEvents(socket,event,world,i+1,x);
            }

        }

        window.clear(sf::Color::Black);

        world.Step(timeStep, velocityIterations, positionIterations);




        game.updateMap(window,socket,world,x);


        window.setView(view1);
        median = getMedian(game.players);
        view1.setCenter(sf::Vector2f(median.first,median.second));
        view1.setSize(zoom(game.players[0],game.players[1],window));

        window.display();


    }

    return 0;
}
