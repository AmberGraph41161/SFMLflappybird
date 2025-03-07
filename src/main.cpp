//as of Thursday, March 06, 2025, 21:57:15
//start SFML 2.x to SFML 3.x migration...
	//https://www.sfml-dev.org/tutorials/3.0/getting-started/migrate/#fixed-width-integers
//finally finished SFML 2.x to SFML 3.x migration as of Thursday, March 06, 2025, 23:10:28

#include <SFML/System/Angle.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>
#include <filesystem>

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Audio/Sound.hpp>

#include "gamefunctions.hpp"
#include "pipe.hpp"
#include "missile.hpp"

int main()
{
	std::cout << "[doing setup crap]" << std::endl;
	srand(time(0));

	const double screenWidth = 1080;
	const double screenHeight = 720;

	//textinput event stuff
	const int captureTextInputEventAsciiLowerBound = 32; //32;
	const int captureTextInputEventAsciiUpperBound = 126;
	const int captureTextInputEventAsciiBackspace = 8;
	const int captureTextInputEventAsciiNewLineFeed = 10;
	const int captureTextInputEventAsciiNewPageFormFeed = 12;
	const int captureTextInputEventAsciiCarriageReturn = 13;
	const int captureTextInputEventAsciiDelete = 127;
	bool captureTextInputEvents = false;
	std::string capturedTextInput = "";

	std::chrono::duration<double> captureTextInputEventBackspaceTickDelta = std::chrono::seconds::zero();
	double captureTextInputEventBackspaceTickDeltaThreshold = 0.1;

	//load player stuff
	sf::Texture playerTexture;
	std::string playerTexturePath = "resources/textures/flappybird.png";
	if(!playerTexture.loadFromFile(playerTexturePath))
	{
		std::cerr << "failed to load \"" << playerTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sprite player(playerTexture);
	
	const sf::Vector2f playerScale(4, 4);
	player.setOrigin(sf::Vector2f(player.getLocalBounds().size.x / 2, player.getLocalBounds().size.y / 2));
	player.setScale(playerScale);


	double defaultPlayerX = screenWidth / 4;
	double defaultPlayerY = screenHeight / 2;
	double playerX = defaultPlayerX;
	double playerY = defaultPlayerY;
	double playerXvelocity = 0;
	double playerYvelocity = 0;
	double gravity = 3000; //1.3
	double antiGravity = gravity * 0.2; //* 0.1
	double playerAngle = 0;
	double playerXfriction = 0.9;
	double playerXspeed = 80000;
	double playerAngleMultiplier = 0.05;
	bool playerInitialJump  = false;
	bool playerJumpedLastTime = false;
	bool playerIsAlive = true;
	bool startMenu = true;
	bool viewHighscoresMenu = false;
	bool pauseMenu = false;
	bool deathMenu = false;
	bool settingsMenu = false;
	
	int playerCurrentScore = 0; //(current score, during live gameplay and not saved score(s))

	//make sure "dat/" exists. if not, create it
	if(!std::filesystem::exists("dat"))
	{
		std::filesystem::create_directory("dat");
	}
	std::map<std::string, int> playerScores;
	getSavedScores(playerScores);
	std::string playerName = getSavedPlayerName();
	int playerNameCharacterLengthThreshold = 30;

	//displaying playerName
	sf::Font masterFont;
	std::string masterFontPath = "resources/fonts/Minecraftia-Regular.ttf";
	if(!masterFont.openFromFile(masterFontPath))
	{
		std::cout << "failed to load\"" << masterFontPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	const int playerNameTextCharacterSize = 24;
	const int playerNameTextLeftMarigin = 10;
	const int playerNameTextTopMarigin = 20;
	bool playerNameTextHoveredOver = false;
	bool playerNameTextClicked = false;
	bool playerNameTextSavedToFile = true;
	sf::Text playerNameText(masterFont);
	playerNameText.setCharacterSize(playerNameTextCharacterSize);
	playerNameText.setPosition(sf::Vector2f(0 + playerNameTextLeftMarigin, 0 + playerNameTextTopMarigin));
	playerNameText.setString(playerName);

	//playerNameTextCursor
	sf::RectangleShape playerNameTextCursor(sf::Vector2f(2, playerNameText.getGlobalBounds().size.y));
	const int playerNameTextCursorLeftMarigin = playerNameTextLeftMarigin + 10;
	const int playerNameTextCursorTopMarigin = playerNameTextTopMarigin;
	playerNameTextCursor.setOrigin(sf::Vector2f(playerNameTextCursor.getLocalBounds().size.x / 2, playerNameTextCursor.getLocalBounds().size.y / 2));
	playerNameTextCursor.setPosition(sf::Vector2f(playerNameText.getGlobalBounds().size.x + playerNameTextCursorLeftMarigin, playerNameTextCursorTopMarigin));
	std::chrono::duration<double> playerNameTextCursorBlinkTickDelta = std::chrono::seconds::zero();
	double playerNameTextCursorBlinkTickDeltaThreshold = 0.7;
	bool playerNameTextCursorBlinkToggle = false;

	//player settings...?
	bool displayHitboxes = false;
	double sfxVolume = 50;
	//double musicVolume = 100;

	//player hitbox?
	sf::RectangleShape playerHitbox(sf::Vector2f(player.getGlobalBounds().size.x * 0.5, player.getGlobalBounds().size.y * 0.7));
	playerHitbox.setOrigin(sf::Vector2f(playerHitbox.getLocalBounds().size.x / 2, playerHitbox.getLocalBounds().size.y / 2));
	playerHitbox.setPosition(sf::Vector2f(playerX, playerY));
	playerHitbox.setFillColor(sf::Color::Red);

	//player jump indicator
	sf::Texture playerJumpIndicatorTexture;
	std::string playerJumpIndicatorTexturePath = "resources/textures/jumpindicator-Sheet.png";
	if(!playerJumpIndicatorTexture.loadFromFile(playerJumpIndicatorTexturePath))
	{
		std::cerr << "failed to load \"" << playerJumpIndicatorTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sprite playerJumpIndicator(playerJumpIndicatorTexture);
	const int playerJumpIndicatorFrameWidth = 25;
	const int playerJumpIndicatorFrameHeight = 37;
	const double playerJumpIndicatorYoffsetFix = -18;

	playerJumpIndicator.setTexture(playerJumpIndicatorTexture);
	playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 0));
	playerJumpIndicator.setOrigin(sf::Vector2f(playerJumpIndicator.getLocalBounds().size.x / 2, playerJumpIndicator.getLocalBounds().size.y / 2));
	playerJumpIndicator.setScale(playerScale);
	playerJumpIndicator.setPosition(sf::Vector2f(playerX, playerY + playerJumpIndicatorYoffsetFix));
	
	std::chrono::duration<double> animatePlayerJumpIndicatorDuration = std::chrono::seconds::zero();
	double animatePlayerJumpIndicatorDurationThreshold = 0.4;

	//load pipe stuff pipes
	sf::Texture pipeTexture;
	//std::string pipeTexturePath = "resources/textures/longpipe.png";
	std::string pipeTexturePath = "resources/textures/christmaslongpipe.png";
	if(!pipeTexture.loadFromFile(pipeTexturePath))
	{
		std::cerr << "failed to load \"" << pipeTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::vector<Pipe> pipes;
	double defaultPipeSpacing = 100;
	double defaultPipeSpeed = 500; //0.2
	bool pipesSubroutine = false;

	pipes.emplace_back(Pipe(pipeTexture, screenWidth - 100, screenHeight / 2, defaultPipeSpacing, defaultPipeSpeed));

	//load missile stuff
	sf::Texture missileTexture;
	//std::string missileTexturePath = "resources/textures/missile-Sheet.png";
	std::string missileTexturePath = "resources/textures/christmasmissile-Sheet.png";
	if(!missileTexture.loadFromFile(missileTexturePath))
	{
		std::cerr << "Failed to load \"" << missileTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	double defaultMissileSpeed = 1500;
	std::vector<Missile> missiles;
	//missiles.emplace_back(Missile(&missileTexture, (screenWidth / 4) * 3, screenHeight / 2, defaultMissileSpeed));

	//load background and flooring stuff
	sf::Texture backgroundTexture;
	//std::string backgroundTexturePath = "resources/textures/slicebackground.png";
	std::string backgroundTexturePath = "resources/textures/christmasslicebackground1.png";
	if(!backgroundTexture.loadFromFile(backgroundTexturePath))
	{
		std::cerr << "failed to load \"" << backgroundTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sprite background(backgroundTexture);
	const sf::Vector2f backgroundScale(7.5, 7.5);
	int backgroundTextureMultiplier = 8;
	backgroundTexture.setRepeated(true);
	background.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(backgroundTexture.getSize().x * backgroundTextureMultiplier, backgroundTexture.getSize().y)));
	background.setTexture(backgroundTexture);
	background.setScale(backgroundScale);
	background.setOrigin(sf::Vector2f(background.getLocalBounds().size.x / 2, background.getLocalBounds().size.y / 2));
	background.setPosition(sf::Vector2f(screenWidth / 2, screenHeight / 2));
	double backgroundOriginalX = background.getPosition().x;
	double backgroundOriginalY = background.getPosition().y;
	double backgroundSpeed = 50;

	//ceiling
	sf::RectangleShape ceiling(sf::Vector2f(screenWidth, screenHeight * 0.03));
	ceiling.setFillColor(sf::Color::Cyan);

	//floor
	sf::RectangleShape floor(sf::Vector2f(screenWidth, screenHeight * 0.1));
	floor.setPosition(sf::Vector2f(0, screenHeight - floor.getLocalBounds().size.y));
	floor.setFillColor(sf::Color::Cyan);

	//leftWall
	const int leftWallThickness = 10;
	sf::RectangleShape leftWall(sf::Vector2f(leftWallThickness, screenHeight));
	leftWall.setFillColor(sf::Color::Cyan);

	//rightWall
	const int rightWallThickness = 10;
	sf::RectangleShape rightWall(sf::Vector2f(rightWallThickness, screenHeight));
	rightWall.setPosition(sf::Vector2f(screenWidth - rightWallThickness, 0));
	rightWall.setFillColor(sf::Color::Cyan);
	
	//score
	const sf::Vector2f scoreTextScale(1, 1);
	sf::Text currentScoreText(masterFont);
	currentScoreText.setCharacterSize(50);
	currentScoreText.setScale(scoreTextScale);
	currentScoreText.setString(std::to_string(playerCurrentScore));
	currentScoreText.setOrigin(sf::Vector2f(currentScoreText.getLocalBounds().size.x / 2, currentScoreText.getLocalBounds().size.y / 2));
	currentScoreText.setPosition(sf::Vector2f(screenWidth / 2, screenHeight / 10));
	
	//start menu
	sf::Texture startButtonTexture;
	std::string startButtonTexturePath = "resources/textures/startbutton-Sheet.png";
	if(!startButtonTexture.loadFromFile(startButtonTexturePath))
	{
		std::cerr << "failed to load \"" << startButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sprite startButton(startButtonTexture);
	const sf::Vector2f startButtonScale(10, 10);
	const int startButtonFrameWidth = 33;
	const int startButtonFrameHeight = 13;
	bool startButtonHoveredOver = false;
	bool startButtonClicked = false;
	int startButtonFlashAnimationCount = 0;
	const int startButtonFlashAnimationCountThreshold = 5;
	std::chrono::duration<double> startButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
	double startButtonFlashAnimationTickDeltaThreshold = 0.08;

	startButton.setTexture(startButtonTexture);
	startButton.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(startButtonFrameWidth, startButtonFrameHeight)));
	startButton.setScale(startButtonScale);
	startButton.setOrigin(sf::Vector2f(startButton.getLocalBounds().size.x / 2, startButton.getLocalBounds().size.y / 2));
	startButton.setPosition(sf::Vector2f(screenWidth / 2, (screenHeight / 5) * 2));

	//viewHighscoresButton stuff
	sf::Texture viewHighscoresButtonTexture;
	std::string viewHighscoresButtonTexturePath = "resources/textures/viewhighscoresbutton-Sheet.png";
	if(!viewHighscoresButtonTexture.loadFromFile(viewHighscoresButtonTexturePath))
	{
		std::cerr << "failed to load \"" << viewHighscoresButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sprite viewHighscoresButton(viewHighscoresButtonTexture);
	const sf::Vector2f viewHighscoresButtonScale(10, 10);
	const int viewHighscoresButtonFrameWidth = 93;
	const int viewHighscoresButtonFrameHeight = 13;
	bool viewHighscoresButtonHoveredOver = false;
	bool viewHighscoresButtonClicked = false;
	int viewHighscoresButtonFlashAnimationCount = 0;
	const int viewHighscoresButtonFlashAnimationCountThreshold = 3;
	std::chrono::duration<double> viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
	double viewHighscoresButtonFlashAnimationTickDeltaThreshold = 0.08;
	const int viewHighscoresButtonPosition0x = screenWidth / 2;
	const int viewHighscoresButtonPosition0y = (screenHeight / 5) * 3;
	const int viewHighscoresButtonPosition1x = screenWidth / 2;
	const int viewHighscoresButtonPosition1y = screenHeight / 8;

	viewHighscoresButton.setTexture(viewHighscoresButtonTexture);
	viewHighscoresButton.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight)));
	viewHighscoresButton.setScale(viewHighscoresButtonScale);
	viewHighscoresButton.setOrigin(sf::Vector2f(viewHighscoresButton.getLocalBounds().size.x / 2, viewHighscoresButton.getLocalBounds().size.y / 2));
	viewHighscoresButton.setPosition(sf::Vector2f(viewHighscoresButtonPosition0x, viewHighscoresButtonPosition0y));

	sf::RectangleShape viewHighscoresBackboardRect(sf::Vector2f(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight * 7));
	viewHighscoresBackboardRect.setFillColor(sf::Color::Black);
	viewHighscoresBackboardRect.setScale(viewHighscoresButtonScale);
	viewHighscoresBackboardRect.setOrigin(sf::Vector2f(viewHighscoresBackboardRect.getLocalBounds().size.x / 2, viewHighscoresBackboardRect.getLocalBounds().size.y / 2));
	viewHighscoresBackboardRect.setPosition(sf::Vector2f(screenWidth / 2, viewHighscoresButtonPosition1y * 7));

	const int viewHighscoresTextLeftMarigin = 20;
	const int viewHighscoresTextTopMarigin = 40;
	sf::Text viewHighscoresText(masterFont);
	viewHighscoresText.setCharacterSize(24);
	viewHighscoresText.setPosition(sf::Vector2f(
			viewHighscoresBackboardRect.getGlobalBounds().position.x + viewHighscoresTextLeftMarigin,
			viewHighscoresBackboardRect.getGlobalBounds().position.y + viewHighscoresTextTopMarigin
			));

	//death menu & high score screen
	sf::Texture playAgainButtonTexture;
	std::string playAgainButtonTexturePath = "resources/textures/playagainbutton-Sheet.png";
	if(!playAgainButtonTexture.loadFromFile(playAgainButtonTexturePath))
	{
		std::cerr << "failed to load \"" << playAgainButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sprite playAgainButton(playAgainButtonTexture);
	const sf::Vector2f playAgainButtonScale(10, 10);
	const int playAgainButtonFrameWidth = 68;
	const int playAgainButtonFrameHeight = 13;
	bool playAgainButtonHoveredOver = false;
	std::chrono::duration<double> playAgainButtonIdleAnimationTickDelta = std::chrono::seconds::zero();
	double playAgainButtonIdleAnimationTickDeltaThreshold = 0.6;

	playAgainButton.setTexture(playAgainButtonTexture);
	playAgainButton.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(playAgainButtonFrameWidth, playAgainButtonFrameHeight)));
	playAgainButton.setScale(playAgainButtonScale);
	playAgainButton.setOrigin(sf::Vector2f(playAgainButton.getLocalBounds().size.x / 2, playAgainButton.getLocalBounds().size.y / 2));
	playAgainButton.setPosition(sf::Vector2f(screenWidth / 2, screenHeight / 2));

	//dimscreen rectangleshape object for death menu, pause menu, etc, etc
	const int dimScreenShapeDefaultDim = 200;
	sf::RectangleShape dimScreenShape(sf::Vector2f(screenWidth, screenHeight));
	dimScreenShape.setFillColor(sf::Color(0, 0, 0, dimScreenShapeDefaultDim));
	sf::Text dimScreenText(masterFont); //note to self as ofThursday, October 17, 2024, 14:14:38 --> make this default font...? change def
	
	sf::Texture quitButtonTexture;
	std::string quitButtonTexturePath = "resources/textures/quitbutton-Sheet.png";
	if(!quitButtonTexture.loadFromFile(quitButtonTexturePath))
	{
		std::cerr << "failed to load \"" << quitButtonTexturePath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sprite quitButton(quitButtonTexture);
	const sf::Vector2f quitButtonScale(10, 10);
	const int quitButtonFrameWidth = 29;
	const int quitButtonFrameHeight = 13;
	bool quitButtonHoveredOver = false;
	bool playDeathAnimationBeforeQuitToStartMenu = false;

	quitButton.setTexture(quitButtonTexture);
	quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 0));
	quitButton.setScale(quitButtonScale);
	quitButton.setOrigin(sf::Vector2f(quitButton.getLocalBounds().size.x / 2, quitButton.getLocalBounds().size.y / 2));
	quitButton.setPosition(sf::Vector2f(screenWidth / 2, (screenHeight / 4) * 3));

	//load music and sound effects sfx
	float masterVolume = 40;
	std::vector<sf::Sound*> allSFXvector;

	sf::SoundBuffer scoreSFXbuffer;
	std::string scoreSFXbufferPath = "resources/sounds/score1.wav";
	if(!scoreSFXbuffer.loadFromFile(scoreSFXbufferPath))
	{
		std::cerr << "failed to load \"" << scoreSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sound scoreSFX(scoreSFXbuffer);
	scoreSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&scoreSFX);

	sf::SoundBuffer menu0SFXbuffer;
	std::string menu0SFXbufferPath = "resources/sounds/menu3.wav";
	if(!menu0SFXbuffer.loadFromFile(menu0SFXbufferPath))
	{
		std::cerr << "failed to load \"" << menu0SFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sound menu0SFX(menu0SFXbuffer);
	menu0SFX.setVolume(sfxVolume);
	allSFXvector.push_back(&menu0SFX);

	sf::SoundBuffer menu1SFXbuffer;
	std::string menu1SFXbufferPath = "resources/sounds/menu4.wav";
	if(!menu1SFXbuffer.loadFromFile(menu1SFXbufferPath))
	{
		std::cerr << "failed to load \"" << menu1SFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	sf::Sound menu1SFX(menu1SFXbuffer);
	menu1SFX.setVolume(sfxVolume);
	allSFXvector.push_back(&menu1SFX);

	sf::SoundBuffer jumpSFXbuffer;
	std::string jumpSFXbufferPath = "resources/sounds/jump0.wav";
	if(!jumpSFXbuffer.loadFromFile(jumpSFXbufferPath))
	{
		std::cerr << "failed to load \"" << jumpSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sound jumpSFX(jumpSFXbuffer);
	jumpSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&jumpSFX);

	sf::SoundBuffer deadSFXbuffer;
	std::string deadSFXbufferPath = "resources/sounds/dead0.wav";
	if(!deadSFXbuffer.loadFromFile(deadSFXbufferPath))
	{
		std::cerr << "failed to load \"" << deadSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sound deadSFX(deadSFXbuffer);
	deadSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&deadSFX);

	sf::SoundBuffer missileDroppingSFXbuffer;
	std::string missileDroppingSFXbufferPath = "resources/sounds/missiledropping0.wav";
	if(!missileDroppingSFXbuffer.loadFromFile(missileDroppingSFXbufferPath))
	{
		std::cerr << "failed to load \"" << missileDroppingSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sound missileDroppingSFX(missileDroppingSFXbuffer);
	missileDroppingSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&missileDroppingSFX);
	
	sf::SoundBuffer missileLaunchingSFXbuffer;
	std::string missileLaunchingSFXbufferPath = "resources/sounds/missilelaunching0.wav";
	if(!missileLaunchingSFXbuffer.loadFromFile(missileLaunchingSFXbufferPath))
	{
		std::cerr << "failed to load \"" << missileLaunchingSFXbufferPath << "\"" << std::endl;
		exit(EXIT_FAILURE);
	}
	sf::Sound missileLaunchingSFX(missileLaunchingSFXbuffer);
	missileLaunchingSFX.setVolume(sfxVolume);
	allSFXvector.push_back(&missileLaunchingSFX);

	for(int x = 0; x < allSFXvector.size(); x++)
	{
		allSFXvector[x]->setVolume(masterVolume);
	}

	//debug FPS font stuff
	bool drawFPS = false;
	sf::Text fps(masterFont);
	fps.setPosition(sf::Vector2f(10, 100));
	fps.setString("0");

	//chrono delta time stuff
	std::chrono::time_point<std::chrono::system_clock> lastlastframe = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::system_clock> lastframe = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> deltaTime = lastframe - lastlastframe;

	//view and window render stuff
	sf::View view(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(screenWidth, screenHeight)));

	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(screenWidth, screenHeight)), "title goes here", sf::Style::Default);
	window.setView(view);
	window.setKeyRepeatEnabled(false); //reread documentation for this one buddy Thursday, October 10, 2024, 10:34:00
	window.setFramerateLimit(200);

	while(window.isOpen())
	{
		while(const std::optional event = window.pollEvent())
		{
			if(event->is<sf::Event::Closed>())
			{
				window.close();
			}
			if(event->is<sf::Event::Resized>())
			{
				//as of Saturday, October 12, 2024, 12:48:52,
					//I have finally gotten the window resizing crap to work.
					//yes, I admit it, all the paranthese make it look garbage, but I don't care.
					//it works, and thats what matters

				if((((float)window.getSize().x / 16) * 9) > window.getSize().y)
				{
					view.setViewport(sf::FloatRect(
						sf::Vector2f(0.5 - (((((float)window.getSize().y / 9) * 16) / (float)window.getSize().x) / 2), 0),
						sf::Vector2f((((float)window.getSize().y / 9) * 16) / (float)window.getSize().x, 1)
					));
				} else
				{
					view.setViewport(sf::FloatRect(
						sf::Vector2f(0, 0.5 - (((((float)window.getSize().x / 16) * 9) / (float)window.getSize().y) / 2)),
						sf::Vector2f(1, (((float)window.getSize().x / 16) * 9) / (float)window.getSize().y)
					));
				}
				window.setView(view);
			}
			
			//thank you internet
				//"https://stackoverflow.com/questions/54681508/how-can-i-add-a-sort-of-text-box-in-sfml-using-keyboard-input-and-sftext-to-di"
				//"https://en.sfml-dev.org/forums/index.php?topic=19965.0"
			if(event->is<sf::Event::TextEntered>())
			{
				if(captureTextInputEvents)
				{
					//wtf ugly. fix later. Tuesday, November 26, 2024, 14:26:10
					//as of Thursday, March 06, 2025, 22:41:17, event uglier with the update of SFML 3.x kill me
					const sf::Event::TextEntered *textEntered = event->getIf<sf::Event::TextEntered>();
					if(
						textEntered->unicode >= captureTextInputEventAsciiLowerBound &&
						textEntered->unicode <= captureTextInputEventAsciiUpperBound
					)
					{
						capturedTextInput += static_cast<char>(textEntered->unicode);
					} else if(
								(textEntered->unicode == captureTextInputEventAsciiBackspace) ||
								(textEntered->unicode == captureTextInputEventAsciiDelete)
							)
					{
						if(capturedTextInput.size() > 0)
						{
							capturedTextInput.pop_back();
						}
					} else if(
								(textEntered->unicode == captureTextInputEventAsciiNewLineFeed) ||
								(textEntered->unicode == captureTextInputEventAsciiNewPageFormFeed) ||
								(textEntered->unicode == captureTextInputEventAsciiCarriageReturn)
							)
					{
						captureTextInputEvents = false;
					}
				} else
				{
					capturedTextInput = "";
				}
			}
		}

		if(captureTextInputEvents)
		{
			if(captureTextInputEventBackspaceTickDelta.count() >= captureTextInputEventBackspaceTickDeltaThreshold)
			{
				if(capturedTextInput.size() > 0)
				{
					capturedTextInput.pop_back();
				}
				captureTextInputEventBackspaceTickDelta = std::chrono::seconds::zero();
			} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Backspace))
			{
				captureTextInputEventBackspaceTickDelta += deltaTime;
			} else
			{
				captureTextInputEventBackspaceTickDelta = std::chrono::seconds::zero();
			}
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q))
		{
			window.close();
		}

		//startscreen and or pause menu
		if(startMenu)
		{
			//playerNameText logic
			if(playerNameText.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))
			{
				if(!playerNameTextHoveredOver && !playerNameTextClicked)
				{
					playerNameText.setCharacterSize(playerNameTextCharacterSize + 10);
					menu0SFX.play();
				}
				playerNameTextHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					if(!playerNameTextClicked)
					{
						menu1SFX.play();
						playerNameTextClicked = true;
						captureTextInputEvents = true;
						playerNameTextSavedToFile = false;
						playerNameText.setFillColor(sf::Color(200, 230, 255, 255));
					}
				}
			} else
			{
				playerNameTextHoveredOver = false;

				if(!playerNameTextHoveredOver)
				{
					playerNameText.setCharacterSize(playerNameTextCharacterSize);
				}
			}

			if(captureTextInputEvents)
			{
				playerName = capturedTextInput;
				if(playerName.size() >= playerNameCharacterLengthThreshold)
				{
					playerName = playerName.substr(0, playerNameCharacterLengthThreshold);
					capturedTextInput = playerName;
				}

				playerNameText.setString(playerName);
			} else if(!playerNameTextSavedToFile)
			{
				capturedTextInput = "";

				if(playerName == "")
				{
					playerName = "player0";
					playerNameText.setString(playerName);
				}
				savePlayerName(playerName);
				playerNameTextSavedToFile = true;
				playerNameTextClicked = false;

				playerNameText.setFillColor(sf::Color::White);
			}

			//startButton logic
				//I should prolly just throw all this into a single button class... kill me. Tuesday, November 26, 2024, 10:40:49
			if(startButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))) //view is in coords, without view is in pixels
			{
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 1));

				if(!startButtonHoveredOver && !startButtonClicked && !viewHighscoresButtonClicked)
				{
					menu0SFX.play();
				}
				startButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					if(!startButtonClicked && !viewHighscoresButtonClicked && !playerNameTextClicked)
					{
						startButtonClicked = true;
						menu1SFX.play();
					}
				}

			} else
			{
				startButtonHoveredOver = false;
				startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 0));
			}

			if(startButtonClicked)
			{
				if(startButtonFlashAnimationCount >= startButtonFlashAnimationCountThreshold)
				{
					startButtonFlashAnimationTickDelta = std::chrono::seconds::zero();

					startButtonFlashAnimationCount = 0;
					startButtonClicked = false;
					startMenu = false;
					playerIsAlive = true;
					continue;
				} else
				{
					if(startButtonFlashAnimationTickDelta.count() >= startButtonFlashAnimationTickDeltaThreshold)
					{
						startButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
						startButtonFlashAnimationCount++;
					} else
					{
						startButtonFlashAnimationTickDelta += deltaTime;
					}

					startButton.setTextureRect(spriteSheetFrame(startButtonFrameWidth, startButtonFrameHeight, 2 + (startButtonFlashAnimationCount % 2)));
				}
			}

			//viewHighscoresButton logic
			if(viewHighscoresButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))
			{
				viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 1));

				if(!viewHighscoresButtonHoveredOver && !viewHighscoresButtonClicked && !startButtonClicked)
				{
					menu0SFX.play();
				}
				viewHighscoresButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					if(!viewHighscoresButtonClicked && !startButtonClicked && !playerNameTextClicked)
					{
						viewHighscoresButtonClicked = true;
						menu1SFX.play();
					}
				}

			} else
			{
				viewHighscoresButtonHoveredOver = false;
				viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 0));
			}

			if(viewHighscoresButtonClicked)
			{
				if(viewHighscoresButtonFlashAnimationCount >= viewHighscoresButtonFlashAnimationCountThreshold)
				{
					viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();

					std::string tempViewHighscoresBackboardText = "";
					getSavedScores(playerScores);
					for(std::map<std::string, int>::iterator it = playerScores.begin(); it != playerScores.end(); ++it)
					{
						tempViewHighscoresBackboardText += it->first + " ---> " + std::to_string(it->second) + "\n";
					}
					viewHighscoresText.setString(tempViewHighscoresBackboardText);

					viewHighscoresButton.setPosition(sf::Vector2f(viewHighscoresButtonPosition1x, viewHighscoresButtonPosition1y));
					viewHighscoresButtonFlashAnimationCount = 0;
					viewHighscoresButtonClicked = false;
					startMenu = false;
					viewHighscoresMenu = true;
					continue;
				} else
				{
					if(viewHighscoresButtonFlashAnimationTickDelta.count() >= viewHighscoresButtonFlashAnimationTickDeltaThreshold)
					{
						viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
						viewHighscoresButtonFlashAnimationCount++;
					} else
					{
						viewHighscoresButtonFlashAnimationTickDelta += deltaTime;
					}

					viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 2 + (viewHighscoresButtonFlashAnimationCount % 2)));
				}
			}
			
			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().size.x / backgroundTextureMultiplier))
			{
				background.setPosition(sf::Vector2f(backgroundOriginalX, backgroundOriginalY));
			}
			background.move(sf::Vector2f(-1 * backgroundSpeed * deltaTime.count(), 0));
			window.draw(background);

			if(playerNameTextClicked)
			{
				playerNameTextCursor.setPosition(sf::Vector2f(playerNameText.getGlobalBounds().size.x + playerNameTextCursorLeftMarigin, playerNameTextCursorTopMarigin));
				
				if(playerNameTextCursorBlinkTickDelta.count() >= playerNameTextCursorBlinkTickDeltaThreshold)
				{
					playerNameTextCursorBlinkTickDelta = std::chrono::seconds::zero();
					if(playerNameTextCursorBlinkToggle)
					{
						playerNameTextCursorBlinkToggle = false;
					} else
					{
						playerNameTextCursorBlinkToggle = true;
					}
				} else
				{
					playerNameTextCursorBlinkTickDelta += deltaTime;
				}
			}

			if(playerNameTextCursorBlinkToggle)
			{
				window.draw(playerNameTextCursor);
			}
			window.draw(playerNameText);
			window.draw(startButton);
			window.draw(viewHighscoresButton);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

		} else if(viewHighscoresMenu)
		{
			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().size.x / backgroundTextureMultiplier))
			{
				background.setPosition(sf::Vector2f(backgroundOriginalX, backgroundOriginalY));
			}
			background.move(sf::Vector2f(-1 * backgroundSpeed * deltaTime.count(), 0));
			window.draw(background);

			//viewHighscoresButton logic
			if(viewHighscoresButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))
			{
				viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 1));

				if(!viewHighscoresButtonHoveredOver && !viewHighscoresButtonClicked && !startButtonClicked)
				{
					menu0SFX.play();
				}
				viewHighscoresButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					if(!viewHighscoresButtonClicked && !startButtonClicked)
					{
						viewHighscoresButtonClicked = true;
						menu1SFX.play();
					}
				}

			} else
			{
				viewHighscoresButtonHoveredOver = false;
				viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 0));
			}

			if(viewHighscoresButtonClicked)
			{
				if(viewHighscoresButtonFlashAnimationCount >= viewHighscoresButtonFlashAnimationCountThreshold)
				{
					viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();

					viewHighscoresButton.setPosition(sf::Vector2f(viewHighscoresButtonPosition0x, viewHighscoresButtonPosition0y));
					viewHighscoresButtonFlashAnimationCount = 0;
					viewHighscoresButtonClicked = false;
					startMenu = true;
					viewHighscoresMenu = false;
					continue;
				} else
				{
					if(viewHighscoresButtonFlashAnimationTickDelta.count() >= viewHighscoresButtonFlashAnimationTickDeltaThreshold)
					{
						viewHighscoresButtonFlashAnimationTickDelta = std::chrono::seconds::zero();
						viewHighscoresButtonFlashAnimationCount++;
					} else
					{
						viewHighscoresButtonFlashAnimationTickDelta += deltaTime;
					}

					viewHighscoresButton.setTextureRect(spriteSheetFrame(viewHighscoresButtonFrameWidth, viewHighscoresButtonFrameHeight, 2 + (viewHighscoresButtonFlashAnimationCount % 2)));
				}
			}
			window.draw(viewHighscoresButton);

			//draw highscores and stuff
			window.draw(viewHighscoresBackboardRect);
			window.draw(viewHighscoresText);
			window.draw(playerNameText);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;
		} else if(pauseMenu || settingsMenu)
		{
			//quitButton logic for pausemenu
			if(quitButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))
			{
				quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 1));

				if(!quitButtonHoveredOver)
				{
					menu0SFX.play();
				}
				quitButtonHoveredOver = true;
			} else
			{
				quitButtonHoveredOver = false;
				quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 0));
			}

			if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				if(quitButtonHoveredOver)
				{
					quitButtonHoveredOver = false;
					playDeathAnimationBeforeQuitToStartMenu = true;
					playerIsAlive = false;
					playerInitialJump = false;
				}
				pauseMenu = false;
				settingsMenu = false;
				menu1SFX.play();
			}

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			window.draw(background);
			
			window.draw(player);
			if(!playerInitialJump)
			{
				window.draw(playerJumpIndicator);
			}
			
			for(int x = 0; x < pipes.size(); x++)
			{
				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());
				
				if(displayHitboxes)
				{
					window.draw(pipes[x].getTopPipeHitbox());
					window.draw(pipes[x].getBottomPipeHitbox());
				}
			}

			for(int x = 0; x < missiles.size(); x++)
			{
				window.draw(missiles[x].getMissile());

				if(displayHitboxes)
				{
					window.draw(missiles[x].getMissileHitbox());
				}
			}

			if(displayHitboxes)
			{
				window.draw(ceiling);
				window.draw(floor);
				window.draw(leftWall);
				window.draw(rightWall);
				window.draw(playerHitbox);
			}

			window.draw(playerNameText);
			window.draw(currentScoreText);
			if(drawFPS)
			{
				window.draw(fps);
			}
			
			window.draw(dimScreenShape);
			dimScreenText.setString("Left Mouse Button Anywhere To Resume");
			dimScreenText.setOrigin(sf::Vector2f(dimScreenText.getLocalBounds().size.x / 2, dimScreenText.getLocalBounds().size.y / 2));
			dimScreenText.setPosition(sf::Vector2f(screenWidth / 2, screenHeight / 2));
			dimScreenText.setCharacterSize(30);
			window.draw(dimScreenText);
			window.draw(quitButton);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

		} else if(deathMenu)
		{
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
			
			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().size.x / backgroundTextureMultiplier))
			{
				background.setPosition(sf::Vector2f(backgroundOriginalX, backgroundOriginalY));
			}
			background.move(sf::Vector2f(-1 * backgroundSpeed * deltaTime.count(), 0));
			window.draw(background);

			//playAgainButton logic
			if(playAgainButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view))) //view is in coords, without view is in pixels
			{
				playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 2));

				if(!playAgainButtonHoveredOver)
				{
					menu0SFX.play();
				}
				playAgainButtonHoveredOver = true;

				if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					menu1SFX.play();
					deathMenu = false;
					playerIsAlive = true;
					playerInitialJump = false;
					continue;
				}
			} else
			{
				playAgainButtonHoveredOver = false;
				if(playAgainButtonIdleAnimationTickDelta.count() >= playAgainButtonIdleAnimationTickDeltaThreshold * 2)
				{
					playAgainButtonIdleAnimationTickDelta = std::chrono::seconds::zero();
				} else if(playAgainButtonIdleAnimationTickDelta.count() >= playAgainButtonIdleAnimationTickDeltaThreshold)
				{
					playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 1));
				} else
				{
					playAgainButton.setTextureRect(spriteSheetFrame(playAgainButtonFrameWidth, playAgainButtonFrameHeight, 0));
				}
				playAgainButtonIdleAnimationTickDelta += deltaTime;
			}

			//quitButton logic for play again menu
			if(quitButton.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window), view)))
			{
				quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 1));

				if(!quitButtonHoveredOver)
				{
					menu0SFX.play();
				}
				quitButtonHoveredOver = true;
				
				if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					quitButtonHoveredOver = false;
					playerIsAlive = true;
					playerInitialJump = false;
					deathMenu = false;
					startMenu = true;
					menu1SFX.play();
				}
			} else
			{
				quitButtonHoveredOver = false;
				quitButton.setTextureRect(spriteSheetFrame(quitButtonFrameWidth, quitButtonFrameHeight, 0));
			}


			window.draw(playAgainButton);
			window.draw(quitButton);
			
			window.draw(playerNameText);

			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;
		} else if(playerIsAlive && !deathMenu)//the game (maybe this is a bad idea...?)
		{
			//pause menu...?
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape))
			{
				pauseMenu = true;
			}

			//player controls
			if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space) && playerJumpedLastTime)
			{
				playerJumpedLastTime = false;
			}

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space) && !playerJumpedLastTime && playerIsAlive)
			{
				playerInitialJump = true;
				playerJumpedLastTime = true;
				playerYvelocity = -1 * antiGravity;

				jumpSFX.play();
			} else if(
				!playerHitbox.getGlobalBounds().findIntersection(floor.getGlobalBounds()).has_value() &&
				!playerHitbox.getGlobalBounds().findIntersection(ceiling.getGlobalBounds()).has_value() &&
				playerInitialJump
			)
			{
				//move player down due to gravity (falling)
				playerYvelocity += gravity * deltaTime.count();
			} else
			{
				//player is touching either floor or ceiling maybe? so check?
				playerYvelocity = 0;

				//ceiling?
				if(playerHitbox.getGlobalBounds().findIntersection(ceiling.getGlobalBounds()).has_value())
				{
					//playerY = ceiling.getGlobalBounds().height + player.getGlobalBounds().top;
					playerYvelocity = 100;
				} else if(
					playerHitbox.getGlobalBounds().findIntersection(floor.getGlobalBounds()).has_value() ||
					playerHitbox.getPosition().y >= floor.getPosition().y
				)
				{
					playerIsAlive = false;
					deadSFX.play();
					continue;
				}
			}

			if(playerInitialJump)
			{
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
				{
					playerXvelocity += -1 * playerXspeed * deltaTime.count();
				} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
				{
					playerXvelocity += playerXspeed * deltaTime.count();
				}
			}

			if(playerHitbox.getGlobalBounds().findIntersection(leftWall.getGlobalBounds()).has_value())
			{
				playerXvelocity = 100;
			} else if(playerHitbox.getGlobalBounds().findIntersection(rightWall.getGlobalBounds()).has_value())
			{
				playerXvelocity = -100;
			}

			//update player
			playerY += playerYvelocity * deltaTime.count();
			playerX += playerXvelocity * deltaTime.count();
			
			playerXvelocity *= playerXfriction * deltaTime.count();

			playerAngle = playerYvelocity * -1 * playerAngleMultiplier;

			playerHitbox.setPosition(sf::Vector2f(playerX, playerY));
			player.setPosition(sf::Vector2f(playerX, playerY));
			player.setRotation(sf::Angle(sf::degrees(playerAngle)));

			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().size.x / backgroundTextureMultiplier))
			{
				background.setPosition(sf::Vector2f(backgroundOriginalX, backgroundOriginalY));
			}
			background.move(sf::Vector2f(-1 * backgroundSpeed * deltaTime.count(), 0));

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			window.draw(background);
			
			window.draw(player);
			if(!playerInitialJump)
			{
				animatePlayerJumpIndicatorDuration += deltaTime;

				if(animatePlayerJumpIndicatorDuration.count() >= animatePlayerJumpIndicatorDurationThreshold * 3)
				{
					playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 0));
					animatePlayerJumpIndicatorDuration = std::chrono::seconds::zero();
				}else if(animatePlayerJumpIndicatorDuration.count() >= animatePlayerJumpIndicatorDurationThreshold * 2)
				{
					playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 2));
				} else if(animatePlayerJumpIndicatorDuration.count() >= animatePlayerJumpIndicatorDurationThreshold)
				{
					playerJumpIndicator.setTextureRect(spriteSheetFrame(playerJumpIndicatorFrameWidth, playerJumpIndicatorFrameHeight, 1));
				}
				window.draw(playerJumpIndicator);
			}
			
			if(displayHitboxes)
			{
				window.draw(ceiling);
				window.draw(floor);
				window.draw(leftWall);
				window.draw(rightWall);
				window.draw(playerHitbox);
			}

			//update pipes stuff
			if(pipes.size() <= 0 && pipesSubroutine)
			{
				pipesSubroutine = false;
				spawnDefaultRandomPipe(pipes, pipeTexture, screenWidth, screenHeight);
			}
			for(int x = 0; x < pipes.size(); x++)
			{
				if(pipes[x].intersects(playerHitbox.getGlobalBounds()))
				{
					playerIsAlive = false;
					deadSFX.play();
					break;
				}

				if(playerInitialJump)
				{
					pipes[x].move(deltaTime.count());
				}
				if(!pipes[x].hasPastPlayer() && pipes[x].isPastPlayer(playerX))
				{
					playerCurrentScore++;
					currentScoreText.setString(std::to_string(playerCurrentScore));
					scoreSFX.play();
					if(RANDOM(0, 10) == 5 && !pipesSubroutine)
					{
						pipeTunnel(pipes, pipeTexture, RANDOM(0, 20), screenWidth - 100, screenHeight / 2);
						pipesSubroutine = true;
					} else if(RANDOM(0, 15) == 5 && !pipesSubroutine)
					{
						pipeShrinkTunnel(pipes, pipeTexture, RANDOM(0, 20), screenWidth - 100, screenHeight / 2);
						pipesSubroutine = true;
					} else if(RANDOM(0, 1) && !pipesSubroutine)
					{
						missiles.emplace_back(Missile(missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						missiles.emplace_back(Missile(missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						missiles.emplace_back(Missile(missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						missiles.emplace_back(Missile(missileTexture, (screenWidth / 4) * 3.5, playerY + RANDOM(-100, 100), defaultMissileSpeed, &missileDroppingSFX, &missileLaunchingSFX));
						missileDroppingSFX.play();
						pipesSubroutine = true;
					} else if(!pipesSubroutine)
					{
						spawnDefaultRandomPipe(pipes, pipeTexture, screenWidth, screenHeight);
					}
				}

				if(pipes[x].isOffScreenLeftRight(0 - 200, view.getSize().x + 1000))
				{
					pipes.erase(pipes.begin() + x);
					x--;
					continue;
				}

				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());

				if(displayHitboxes)
				{
					window.draw(pipes[x].getTopPipeHitbox());
					window.draw(pipes[x].getBottomPipeHitbox());
				}
			}

			//update missiles stuff
			for(int x = 0; x < missiles.size(); x++)
			{
				if(missiles[x].getMissile().getGlobalBounds().findIntersection(playerHitbox.getGlobalBounds()).has_value())
				{
					playerIsAlive = false;
					deadSFX.play();
					break;
				}

				missiles[x].move(deltaTime.count());
				if(missiles[x].isOffScreenLeftRight(0 - 200, view.getSize().x + 1000))
				{
					missiles.erase(missiles.begin() + x);
					x--;
					continue;
				}

				window.draw(missiles[x].getMissile());

				if(displayHitboxes)
				{
					window.draw(missiles[x].getMissileHitbox());
				}
			}

			window.draw(playerNameText);
			window.draw(currentScoreText);
			if(drawFPS)
			{
				window.draw(fps);
			}
			
			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

			fps.setString(std::to_string(1 / deltaTime.count()) + "\ndeltaTime(): " + std::to_string(deltaTime.count()));
		} else if(!playerIsAlive)
		{
			playerYvelocity += gravity * deltaTime.count();

			playerY += playerYvelocity * deltaTime.count();
			playerX += playerXvelocity * deltaTime.count();

			playerAngle = playerYvelocity * -1 * playerAngleMultiplier;

			playerHitbox.setPosition(sf::Vector2f(playerX, playerY));
			player.setPosition(sf::Vector2f(playerX, playerY));
			player.setRotation(sf::Angle(sf::degrees(playerAngle)));

			//draw and deltaTime
			lastlastframe = std::chrono::high_resolution_clock::now();
			window.clear(sf::Color::Black);
		
			//update background
			if(background.getPosition().x <= backgroundOriginalX - (background.getGlobalBounds().size.x / backgroundTextureMultiplier))
			{
				background.setPosition(sf::Vector2f(backgroundOriginalX, backgroundOriginalY));
			}
			background.move(sf::Vector2f(-1 * backgroundSpeed * deltaTime.count(), 0));
			window.draw(background);

			for(int x = 0; x < pipes.size(); x++)
			{
				window.draw(pipes[x].getTopPipe());
				window.draw(pipes[x].getBottomPipe());
			}
			for(int x = 0; x < missiles.size(); x++)
			{
				window.draw(missiles[x].getMissile());
			}

			window.draw(player);

			window.draw(playerNameText);
			window.draw(currentScoreText);
			if(drawFPS)
			{
				window.draw(fps);
			}
			
			window.display();
			lastframe = std::chrono::high_resolution_clock::now();
			deltaTime = lastframe - lastlastframe;

			if(player.getPosition().y >= screenHeight)
			{
				if(playerScores[playerName] < playerCurrentScore)
				{
					playerScores[playerName] = playerCurrentScore;
					saveScores(playerScores);
				}

				pipes.clear();
				missiles.clear();

				playerCurrentScore = 0;
				playerXvelocity = 0;
				playerYvelocity = 0;
				playerX = defaultPlayerX;
				playerY = defaultPlayerY;

				playerJumpedLastTime = false;

				currentScoreText.setString("0");
				playerHitbox.setPosition(sf::Vector2f(playerX, playerY));
				player.setPosition(sf::Vector2f(playerX, playerY));
				player.setRotation(sf::Angle(sf::degrees(playerAngle)));

				if(playDeathAnimationBeforeQuitToStartMenu)
				{
					playDeathAnimationBeforeQuitToStartMenu = false;
					startMenu = true;
				} else
				{
					deathMenu = true;
				}
				spawnDefaultRandomPipe(pipes, pipeTexture, screenWidth, screenHeight);
			}
		}
	}
	saveScores(playerScores);

	std::cout << "[done!]" << std::endl;
	return 0;
}
