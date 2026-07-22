#include <SFML/Graphics.hpp>
#include "Player.h"
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Author Metadata
const std::string AUTHOR = "Jaira Settles";
const std::string VERSION = "1.0.0";
void initSystem() {
    std::cout << "[" << AUTHOR << " Core Engine v" << VERSION << "] Starting..." << std::endl;
}

enum GameState { MENU, CUTSCENE, PLAYING, FINAL_SCENE, WIN_SCREEN };

struct Debris { sf::RectangleShape shape; float speedModifier; };

struct TalkativeCircle { sf::CircleShape shape; std::string trashTalk; sf::Text speechText; bool isRolling; float rollSpeed; };

struct SentientSpike { sf::ConvexShape shape; std::string text; sf::Text speechText; };

void drawSpeechBubble(sf::RenderWindow& window, const sf::Text& text, sf::Color bgColor) { sf::FloatRect bounds = text.getGlobalBounds(); sf::RectangleShape box(sf::Vector2f(bounds.width + 16.f, bounds.height + 12.f)); box.setPosition(bounds.left - 8.f, bounds.top - 6.f); box.setFillColor(bgColor); box.setOutlineColor(sf::Color::Black); box.setOutlineThickness(1.f); window.draw(box); window.draw(text); }

int main() {
	initSystem();
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	sf::RenderWindow window(sf::VideoMode(800, 600), "Circles Versus Squares: Chronicles");

	window.setFramerateLimit(60);

	sf::View worldView = window.getDefaultView();

	sf::View hudView = window.getDefaultView();

	Player square;

	sf::Clock clock;

	sf::Clock kissClock;

	sf::Clock cutsceneClock;

	sf::Clock coreBlinkClock;

	GameState currentGameState = MENU;

	int currentLevel = 1;

	int cutscenePage = 0;

	int finaleStep = 0;

	bool corePlatformsVisible = true;

	int deathCount = 0;

	bool isGoodEnding = false;

	std::vector<sf::ConvexShape> invadingTriangles;

	int spaceTapsToLand = 0;

	bool spaceWasReleased = true;

	bool isBeingPulledUp = false;

	sf::Font font;

	bool hasFont = font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");

	sf::Text deathText;

	if (hasFont) {

		deathText.setFont(font);

		deathText.setCharacterSize(16);

		deathText.setFillColor(sf::Color::Red);

		deathText.setPosition(15.f, 15.f);

	}

	// --- Background/Aesthetic Elements --- 

	std::vector<sf::ConvexShape> ruins;

	const float groundY = 450.f;

	sf::Color ruinColor(30, 30, 40);

	for (int i = 0; i < 5; ++i) {

		sf::ConvexShape pillar;

		pillar.setPointCount(4);

		float baseX = i * 200.f;

		pillar.setPoint(0, sf::Vector2f(baseX, groundY));

		pillar.setPoint(1, sf::Vector2f(baseX + 40.f, 180.f + (i * 30.f)));

		pillar.setPoint(2, sf::Vector2f(baseX + 90.f, 150.f + (i * 20.f)));

		pillar.setPoint(3, sf::Vector2f(baseX + 130.f, groundY));

		pillar.setFillColor(ruinColor);

		ruins.push_back(pillar);

	}

	// Home screen's tiny floating elements 

	std::vector<Debris> floatingDebris;

	for (int i = 0; i < 15; ++i) {

		Debris d;

		float size = 6.f + (rand() % 10);

		d.shape.setSize(sf::Vector2f(size, size));

		d.shape.setFillColor(sf::Color(0, 255, 200, 80));

		d.shape.setPosition(rand() % 800, rand() % 350);

		d.speedModifier = 0.3f + ((rand() % 6) * 0.1f);

		floatingDebris.push_back(d);

	}

	sf::CircleShape sun(40.f);

	sun.setFillColor(sf::Color(255, 255, 140));

	sun.setPosition(700.f, 50.f);

	sf::RectangleShape cyanMenuSquare(sf::Vector2f(30.f, 30.f));

	cyanMenuSquare.setFillColor(sf::Color::Cyan);

	cyanMenuSquare.setPosition(200.f, 420.f);

	sf::RectangleShape ladyMenuSquare(sf::Vector2f(30.f, 30.f));

	ladyMenuSquare.setFillColor(sf::Color::Magenta);

	ladyMenuSquare.setPosition(500.f, 420.f);

	sf::Text ladyBubbleText;

	if (hasFont) {

		ladyBubbleText.setFont(font);

		ladyBubbleText.setCharacterSize(14);

		ladyBubbleText.setFillColor(sf::Color::Black);

		ladyBubbleText.setPosition(465.f, 355.f);

	}

	sf::Text cyanBubbleText;

	if (hasFont) {

		cyanBubbleText.setFont(font);

		cyanBubbleText.setCharacterSize(14);

		cyanBubbleText.setFillColor(sf::Color::Black);

		cyanBubbleText.setPosition(115.f, 345.f);

	}

	bool kissHasFired = false, kissSequenceFinished = false;

	sf::CircleShape floatingHeart(6.f);

	floatingHeart.setFillColor(sf::Color(255, 105, 180));

	sf::ConvexShape cutsceneTriangle;

	cutsceneTriangle.setPointCount(3);

	cutsceneTriangle.setPoint(0, sf::Vector2f(0.f, 80.f));

	cutsceneTriangle.setPoint(1, sf::Vector2f(45.f, 0.f));

	cutsceneTriangle.setPoint(2, sf::Vector2f(90.f, 80.f));

	cutsceneTriangle.setFillColor(sf::Color(50, 205, 50));

	cutsceneTriangle.setOrigin(45.f, 40.f);

	// --- Story Arrays --- 

	std::vector<std::vector<std::string>> levelLoreTexts = {

	{

	"Greetings, Symmetrical One. I am a defector from the Triangle Resistance.",

	"I know your true identity, Sir Square... the kingdom remembers you as the legendary Knight.",

	"You were destined to marry Princess Lady Square and become our rightful King!",

	"King Circle staged a coup, kidnapping the Princess to siphon her perfect right angles.",

	"Break through Zone 1: The Glitched Cyber-Ruins to track her royal energy trail!\n[Press ENTER to begin Level 1]"

	},

	{

	"Wait, Sir Square! The Princess you saw at the gate... it was a trap!",

	"King Circle used digital frequencies to project a false HOLOGRAM to stall your pursuit.",

	"Worse yet, he used forbidden geometric alchemy to forge the EVIL PURPLE LADY SQUARE.",

	"She is a siren puppet designed to look like royalty and seduce you into dropping your guard.",

	"Do not fall for her illusions inside Zone 2: The Overgrown Bio-Dome!\n[Press ENTER to begin Level 2]"

	},

	{

	"Incredible... you ignored the siren and the hologram. Your knightly resolve is unmatched.",

	"But the final layer stands between a King and his Crown: The Civil War of the Angles.",

	"The spikes filling the fortress are my own brainwashed brethren, the Acute Vanguard cult.",

	"Watch out! The Purple Saboteur has set up a protective radar shield around the engine gears.",

	"If you get close, her LEVITATION spell will pull you into the abyss!",

	"Tap SPACE rapidly to stay grounded!\n[Press ENTER to begin Level 3]"

	},

	{

	"You have scaled the fortress! Look ahead... we have reached the absolute center.",

	"ZONE 4: THE HIGH-TECH NEXUS CORE. The digital grid completely drops away here.",

	"King Circle is using the Core to bleed the Princess's right angles dry in real-time!",

	"He has combined elements from all zones: Glitch shields, spikes, and the Daze puppet.",

	"Worse, the platforms in the Nexus blink completely out of existence on a strict 1.5s clock timer!",

	"Dodge his overhead tracking crown, climb the boundaries, and seal your legacy, SIRE!\n[Press ENTER TO FACE KING CIRCLE]"

	}

	};

	// --- Entities & Map Objects --- 

	std::vector<sf::RectangleShape> platforms;

	std::vector<TalkativeCircle> circleEnemies;

	std::vector<SentientSpike> dynamicSpikes;

	sf::RectangleShape glitchSwitch(sf::Vector2f(25.f, 25.f));

	bool glitchSwitchTriggered = false;

	sf::RectangleShape matrixGate(sf::Vector2f(150.f, 25.f));

	std::vector<sf::CircleShape> bioBouncers;

	std::vector<sf::CircleShape> clockworkGears;

	sf::CircleShape kingCircleBoss(55.f);

	kingCircleBoss.setFillColor(sf::Color(200, 0, 0));

	kingCircleBoss.setOutlineColor(sf::Color::Red);

	kingCircleBoss.setOutlineThickness(3.f);

	bool bossIsDead = false;

	sf::ConvexShape kingCrown;

	kingCrown.setPointCount(5);

	kingCrown.setPoint(0, sf::Vector2f(0.f, 35.f));

	kingCrown.setPoint(1, sf::Vector2f(15.f, 0.f));

	kingCrown.setPoint(2, sf::Vector2f(35.f, 20.f));

	kingCrown.setPoint(3, sf::Vector2f(55.f, 0.f));

	kingCrown.setPoint(4, sf::Vector2f(70.f, 35.f));

	kingCrown.setFillColor(sf::Color::Yellow);

	kingCrown.setOutlineColor(sf::Color(218, 165, 32));

	kingCrown.setOutlineThickness(2.f);

	sf::Text bossSpeechText;

	if (hasFont) {

		bossSpeechText.setFont(font);

		bossSpeechText.setCharacterSize(13);

		bossSpeechText.setFillColor(sf::Color::Red);

	}

	sf::ConvexShape levelStartTriangle;

	levelStartTriangle.setPointCount(3);

	levelStartTriangle.setPoint(0, sf::Vector2f(0.f, 26.f));

	levelStartTriangle.setPoint(1, sf::Vector2f(13.f, 0.f));

	levelStartTriangle.setPoint(2, sf::Vector2f(26.f, 26.f));

	levelStartTriangle.setFillColor(sf::Color(50, 205, 50));

	sf::Text informantSpeech;

	if (hasFont) {

		informantSpeech.setFont(font);

		informantSpeech.setCharacterSize(11);

		informantSpeech.setFillColor(sf::Color(50, 205, 50));

	}

	sf::RectangleShape ladyGoal(sf::Vector2f(30.f, 30.f));

	ladyGoal.setFillColor(sf::Color::Magenta);

	sf::Text ladyGoalSpeech;

	if (hasFont) {

		ladyGoalSpeech.setFont(font);

		ladyGoalSpeech.setCharacterSize(13);

		ladyGoalSpeech.setFillColor(sf::Color::Magenta);

	}

	sf::RectangleShape evilPurpleLady(sf::Vector2f(30.f, 30.f));

	evilPurpleLady.setFillColor(sf::Color(148, 0, 211));

	sf::Text purpleSpeech;

	if (hasFont) {

		purpleSpeech.setFont(font);

		purpleSpeech.setCharacterSize(13);

		purpleSpeech.setFillColor(sf::Color(148, 0, 211));

	}

	// --- Map Level Loading Logic --- 

	auto loadLevel = [&](int levelNum) {

		platforms.clear();

		circleEnemies.clear();

		dynamicSpikes.clear();

		bioBouncers.clear();

		clockworkGears.clear();

		glitchSwitchTriggered = false;

		coreBlinkClock.restart();

		corePlatformsVisible = true;

		spaceTapsToLand = 0;

		spaceWasReleased = true;

		isBeingPulledUp = false;

		bossIsDead = false;

		kingCircleBoss.setScale(1.f, 1.f);

		auto addPlatform = [&](float x, float y, float w, float h, sf::Color fill, sf::Color outline) {

			sf::RectangleShape plat(sf::Vector2f(w, h));

			plat.setPosition(x, y);

			plat.setFillColor(fill);

			plat.setOutlineColor(outline);

			plat.setOutlineThickness(2.f);

			platforms.push_back(plat);

			};

		auto addCircle = [&](float x, float y, std::string msg, bool rolls, float speed = 0.f) {

			TalkativeCircle tc;

			tc.shape.setRadius(14.f);

			tc.shape.setFillColor(sf::Color::Red);

			tc.shape.setPosition(x, y);

			tc.trashTalk = msg;

			tc.isRolling = rolls;

			tc.rollSpeed = speed;

			if (hasFont) {

				tc.speechText.setFont(font);

				tc.speechText.setCharacterSize(12);

				tc.speechText.setFillColor(sf::Color::Red);

				tc.speechText.setString(msg);

			}

			circleEnemies.push_back(tc);

			};

		auto addSpike = [&](float x, float y, bool upsideDown, std::string taunt) {

			SentientSpike ss;

			ss.shape.setPointCount(3);

			if (!upsideDown) {

				ss.shape.setPoint(0, sf::Vector2f(0.f, 30.f));

				ss.shape.setPoint(1, sf::Vector2f(15.f, 0.f));

				ss.shape.setPoint(2, sf::Vector2f(30.f, 30.f));

			}

			else {

				ss.shape.setPoint(0, sf::Vector2f(0.f, 0.f));

				ss.shape.setPoint(1, sf::Vector2f(15.f, 30.f));

				ss.shape.setPoint(2, sf::Vector2f(30.f, 0.f));

			}

			ss.shape.setFillColor(sf::Color::Red);

			ss.shape.setPosition(x, y);

			ss.text = taunt;

			if (hasFont) {

				ss.speechText.setFont(font);

				ss.speechText.setCharacterSize(10);

				ss.speechText.setFillColor(sf::Color::Red);

				ss.speechText.setString(taunt);

			}

			dynamicSpikes.push_back(ss);

			};

		if (levelNum == 1) {

			sf::Color cyberDark(25, 20, 35);

			sf::Color neonCyan(0, 255, 230);

			addPlatform(-200.f, 500.f, 700.f, 100.f, cyberDark, neonCyan);

			addPlatform(650.f, 500.f, 600.f, 100.f, cyberDark, neonCyan);

			addPlatform(250.f, 380.f, 180.f, 20.f, cyberDark, neonCyan);

			glitchSwitch.setPosition(250.f, 355.f);

			glitchSwitch.setFillColor(sf::Color::Yellow);

			matrixGate.setPosition(500.f, 500.f);

			matrixGate.setFillColor(sf::Color(255, 0, 0, 40));

			matrixGate.setOutlineColor(sf::Color::Red);

			addPlatform(1350.f, 450.f, 900.f, 150.f, cyberDark, neonCyan);

			addCircle(380.f, 352.f, "SYSTEM GLITCH! STAY BACK!", true, -100.f);

			addSpike(1550.f, 420.f, false, "CRITICAL ERROR!");

			addSpike(1800.f, 420.f, false, "SEGFAULT!");

			levelStartTriangle.setPosition(180.f, 474.f);

			informantSpeech.setString("Hit that Yellow Switch to rebuild the bridge!\nWatch out for the rogue patrolling defenses!");

			evilPurpleLady.setPosition(-5000.f, -5000.f);

			purpleSpeech.setString("");

			ladyGoal.setPosition(2100.f, 390.f);

			ladyGoalSpeech.setString("You broke the first firewall! Keep moving! ->");

		}

		else if (levelNum == 2) {

			sf::Color mossGreen(35, 55, 35);

			sf::Color copperRust(210, 105, 30);

			addPlatform(-200.f, 500.f, 600.f, 100.f, mossGreen, copperRust);

			addPlatform(550.f, 500.f, 650.f, 100.f, sf::Color(10, 30, 10), sf::Color::Green);

			addPlatform(550.f, 290.f, 400.f, 40.f, mossGreen, copperRust);

			for (float sx = 650.f; sx < 1100.f; sx += 140.f) {

				addSpike(sx, 470.f, false, "SPIKE PIT!");

			}

			sf::CircleShape pad1(24.f);

			pad1.setFillColor(sf::Color(50, 205, 50));

			pad1.setPosition(450.f, 476.f);

			bioBouncers.push_back(pad1);

			addPlatform(1250.f, 150.f, 60.f, 450.f, mossGreen, sf::Color::Magenta);

			addSpike(1220.f, 470.f, false, "CATCH ME IF YOU CAN!");

			addPlatform(1310.f, 250.f, 500.f, 40.f, mossGreen, copperRust);

			addPlatform(1800.f, 500.f, 500.f, 100.f, mossGreen, copperRust);

			levelStartTriangle.setPosition(100.f, 474.f);

			informantSpeech.setString("Bounce high, find the giant purple-trimmed wall, and scale it using [W]!");

			evilPurpleLady.setPosition(1265.f, 90.f);

			purpleSpeech.setString("Look at you, crawling up my grid like an insect.");

			addCircle(1900.f, 472.f, "NO PASSING ZONE!", true, -180.f);

			ladyGoal.setPosition(2150.f, 420.f);

			ladyGoalSpeech.setString("Hurry! Climb higher next zone! ->");

		}

		else if (levelNum == 3) {

			sf::Color ironGrey(40, 45, 50);

			sf::Color brightBrass(225, 190, 80);

			addPlatform(-200.f, 500.f, 500.f, 100.f, ironGrey, brightBrass);

			addPlatform(500.f, 500.f, 450.f, 100.f, ironGrey, brightBrass);

			addPlatform(1100.f, 500.f, 1200.f, 100.f, ironGrey, brightBrass);

			addPlatform(1150.f, 320.f, 450.f, 40.f, ironGrey, sf::Color(100, 100, 100));

			sf::CircleShape gear1(45.f);

			gear1.setFillColor(brightBrass);

			gear1.setOrigin(45.f, 45.f);

			gear1.setPosition(450.f, 420.f);

			clockworkGears.push_back(gear1);

			sf::CircleShape gear2(60.f);

			gear2.setFillColor(brightBrass);

			gear2.setOrigin(60.f, 60.f);

			gear2.setPosition(720.f, 350.f);

			clockworkGears.push_back(gear2);

			addPlatform(650.f, 250.f, 200.f, 20.f, ironGrey, brightBrass);

			for (float sx = 320.f; sx < 480.f; sx += 45.f) {

				addSpike(sx, 470.f, false, "POINTY!");

			}

			for (float sx = 960.f; sx < 1090.f; sx += 45.f) {

				addSpike(sx, 470.f, false, "OUPH!");

			}

			for (float sx = 1200.f; sx < 1550.f; sx += 50.f) {

				addSpike(sx, 360.f, true, "ACUTE CULT!");

			}

			levelStartTriangle.setPosition(120.f, 474.f);

			informantSpeech.setString("Double-Jump through the gears! Keep deaths < 10 for Good Ending!");

			addCircle(1350.f, 472.f, "King Circle runs this clock!", true, -200.f);

			evilPurpleLady.setPosition(730.f, 220.f);

			purpleSpeech.setString("Hahaha! Gravity is an illusion! Float up into my ceiling hooks!");

			ladyGoal.setPosition(2100.f, 420.f);

			ladyGoalSpeech.setString("You resisted everything! Right angles win! <3");

		}

		else if (levelNum == 4) {

			sf::Color pureBlack(0, 0, 0);

			sf::Color neonRed(255, 0, 50);

			addPlatform(-200.f, 500.f, 450.f, 100.f, pureBlack, neonRed);

			addPlatform(350.f, 420.f, 140.f, 20.f, pureBlack, neonRed);

			glitchSwitch.setPosition(410.f, 395.f);

			glitchSwitch.setFillColor(sf::Color::Yellow);

			matrixGate.setPosition(520.f, 500.f);

			matrixGate.setFillColor(sf::Color(255, 0, 0, 40));

			matrixGate.setOutlineColor(neonRed);

			addPlatform(700.f, 380.f, 150.f, 20.f, pureBlack, sf::Color::Red);

			addPlatform(950.f, 440.f, 150.f, 20.f, pureBlack, sf::Color::Red);

			addPlatform(1200.f, 360.f, 150.f, 20.f, pureBlack, sf::Color::Red);

			addPlatform(1450.f, 500.f, 800.f, 100.f, pureBlack, neonRed);

			for (float sx = 1550.f; sx < 1900.f; sx += 120.f) {

				addSpike(sx, 470.f, false, "FALL DOWN!");

			}

			// Fixed rolling physics range: Extended full tracking bounds for Level 4 circles 

			addCircle(1460.f, 472.f, "DEATH TO THE CRUSADER!", true, 230.f);

			levelStartTriangle.setPosition(50.f, 474.f);

			informantSpeech.setString("The platforms match the 1.5s visual grid! Move quickly!");

			evilPurpleLady.setPosition(1010.f, 410.f);

			purpleSpeech.setString("You can't escape my field logic!");

			ladyGoal.setPosition(2100.f, 420.f);

			ladyGoalSpeech.setString("Break the cage! Save me!");

			bossSpeechText.setString("YOUR SYMMETRY IS BROKEN, KNIGHT!");

			kingCircleBoss.setRadius(55.f);

			kingCircleBoss.setPosition(1000.f, 80.f);

		}

		};

	sf::Text playerSpeech;

	if (hasFont) {

		playerSpeech.setFont(font);

		playerSpeech.setCharacterSize(12);

		playerSpeech.setFillColor(sf::Color::Cyan);

	}

	sf::Vector2f spawnPoint(100.f, 400.f);

	loadLevel(currentLevel);

	sf::Text finaleText;

	if (hasFont) {

		finaleText.setFont(font);

		finaleText.setCharacterSize(14);

		finaleText.setFillColor(sf::Color::White);

	}

	sf::ConvexShape endingTriangleSpeaker;

	endingTriangleSpeaker.setPointCount(3);

	endingTriangleSpeaker.setPoint(0, sf::Vector2f(0.f, 100.f));

	endingTriangleSpeaker.setPoint(1, sf::Vector2f(50.f, 0.f));

	endingTriangleSpeaker.setPoint(2, sf::Vector2f(100.f, 100.f));

	endingTriangleSpeaker.setFillColor(sf::Color(50, 205, 50));

	endingTriangleSpeaker.setPosition(350.f, 80.f);

	// --- Core Engine Window Loop --- 

	while (window.isOpen()) {

		sf::Event event;

		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) window.close();

			if (currentGameState == PLAYING) {

				square.handleInput(event);

			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {

				if (currentGameState == MENU) {

					currentGameState = CUTSCENE;

					cutscenePage = 0;

					cutsceneClock.restart();

					deathCount = 0;

				}

				else if (currentGameState == CUTSCENE) {

					if (cutscenePage < static_cast<int>(levelLoreTexts[currentLevel - 1].size()) - 1) {

						cutscenePage++;

					}

					else {

						square.resetPosition(spawnPoint);

						currentGameState = PLAYING;

					}

				}

				else if (currentGameState == FINAL_SCENE) {

					int maxSteps = isGoodEnding ? 3 : 2;

					if (finaleStep < maxSteps) {

						finaleStep++;

						if (isGoodEnding && finaleStep == 2) {

							invadingTriangles.clear();

							for (int i = 0; i < 35; ++i) {

								sf::ConvexShape tri;

								tri.setPointCount(3);

								tri.setPoint(0, sf::Vector2f(0.f, 30.f));

								tri.setPoint(1, sf::Vector2f(15.f, 0.f));

								tri.setPoint(2, sf::Vector2f(30.f, 30.f));

								tri.setFillColor(sf::Color(34, 139, 34));

								tri.setPosition(static_cast<float>(rand() % 750), static_cast<float>(200 + (rand() % 350)));

								invadingTriangles.push_back(tri);

							}

						}

					}

					else {

						currentGameState = WIN_SCREEN;

					}

				}

				else if (currentGameState == WIN_SCREEN) {

					currentGameState = MENU;

					currentLevel = 1;

					finaleStep = 0;

					loadLevel(currentLevel);

					if (isGoodEnding) {

						// --- Original Good Ending Sub-Loop Injection --- 

						sf::RenderWindow endingWindow(sf::VideoMode(800, 600), "Circles Versus Squares: The True Ending");

						endingWindow.setFramerateLimit(60);

						sf::View hudViewEnd = endingWindow.getDefaultView();

						sf::Clock animationClock;

						enum class GoodEndingState { FINAL_SCENE, WIN_SCREEN };

						GoodEndingState goodState = GoodEndingState::FINAL_SCENE;

						int currentPanel = 0;

						sf::RectangleShape dais(sf::Vector2f(300.f, 30.f));

						dais.setFillColor(sf::Color(40, 40, 50));

						dais.setPosition(250.f, 350.f);

						sf::RectangleShape sirSquare(sf::Vector2f(32.f, 32.f));

						sirSquare.setFillColor(sf::Color::Cyan);

						sirSquare.setPosition(330.f, 318.f);

						sf::RectangleShape princessLady(sf::Vector2f(32.f, 32.f));

						princessLady.setFillColor(sf::Color::Magenta);

						princessLady.setPosition(430.f, 318.f);

						sf::ConvexShape goldCrown;

						goldCrown.setPointCount(5);

						goldCrown.setPoint(0, sf::Vector2f(0.f, 18.f));

						goldCrown.setPoint(1, sf::Vector2f(7.f, 0.f));

						goldCrown.setPoint(2, sf::Vector2f(17.f, 11.f));

						goldCrown.setPoint(3, sf::Vector2f(27.f, 0.f));

						goldCrown.setPoint(4, sf::Vector2f(34.f, 18.f));

						goldCrown.setFillColor(sf::Color::Yellow);

						goldCrown.setOutlineColor(sf::Color(218, 165, 32));

						goldCrown.setOutlineThickness(1.f);

						goldCrown.setOrigin(17.f, 18.f);

						float crownYPosition = 150.f;

						sf::ConvexShape loreTriangle;

						loreTriangle.setPointCount(3);

						loreTriangle.setPoint(0, sf::Vector2f(0.f, 80.f));

						loreTriangle.setPoint(1, sf::Vector2f(40.f, 0.f));

						loreTriangle.setPoint(2, sf::Vector2f(80.f, 80.f));

						loreTriangle.setFillColor(sf::Color(220, 20, 60));

						loreTriangle.setOrigin(40.f, 40.f);

						std::vector<sf::ConvexShape> ambushFleet;

						for (int i = 0; i < 40; ++i) {

							sf::ConvexShape tri;

							tri.setPointCount(3);

							tri.setPoint(0, sf::Vector2f(0.f, 20.f));

							tri.setPoint(1, sf::Vector2f(10.f, 0.f));

							tri.setPoint(2, sf::Vector2f(20.f, 20.f));

							tri.setFillColor(sf::Color(180, 0, 30));

							float angle = (i * 9.f) * 3.14159f / 180.f;

							float radius = 170.f + (rand() % 40);

							tri.setPosition(392.f + std::cos(angle) * radius, 330.f + std::sin(angle) * radius);

							ambushFleet.push_back(tri);

						}

						std::vector<std::string> script = {

						"Princess Lady Square: \"Oh, my hero! You actually made it through the Nexus Core!\"",

						"Sir Square: \"I swore an oath to protect our geometry. I came to save you, Princess.\"",

						"Princess Lady Square: \"We can do this together. I can't wait to go back home\"",

						"Sir Square: \"Wait... what is happening?! Look at the tracking grid!\"",

						"\"UNACCEPTABLE! You squares lounge on comfortable right-angled thrones while my kind were treated as scrap geometry!\"",

						"\"We sacrificed our lives to leak the structural data code to you... and you just take his crown?!\"",

						"\"Triangles are inherently superior. Three points of absolute structural perfection! Squares are nothing without us!\" "

						};

						sf::Text dialogueText;

						if (hasFont) {

							dialogueText.setFont(font);

							dialogueText.setCharacterSize(15);

							dialogueText.setFillColor(sf::Color::White);

						}

						sf::Text continuePrompt;

						if (hasFont) {

							continuePrompt.setFont(font);

							continuePrompt.setCharacterSize(13);

							continuePrompt.setFillColor(sf::Color::Yellow);

							continuePrompt.setString("[Press ENTER to continue]");

						}

						while (endingWindow.isOpen()) {

							sf::Event endEvent;

							while (endingWindow.pollEvent(endEvent)) {

								if (endEvent.type == sf::Event::Closed) endingWindow.close();

								if (endEvent.type == sf::Event::KeyPressed && endEvent.key.code == sf::Keyboard::Enter) {

									if (goodState == GoodEndingState::FINAL_SCENE) {

										if (currentPanel < static_cast<int>(script.size()) - 1) {

											currentPanel++;

											if (currentPanel == 4) {

												crownYPosition = 50.f;

											}

										}

										else {

											goodState = GoodEndingState::WIN_SCREEN;

										}

									}

									else {

										currentPanel = 0;

										crownYPosition = 150.f;

										endingWindow.close();

									}

								}

							}

							float endTimeVal = animationClock.getElapsedTime().asSeconds();

							if (goodState == GoodEndingState::FINAL_SCENE) {

								if (currentPanel < 4) {

									endingWindow.clear(sf::Color(20, 15, 30));

									endingWindow.draw(dais);

									endingWindow.draw(sirSquare);

									endingWindow.draw(princessLady);

									if (currentPanel == 0) {

										goldCrown.setPosition(346.f, crownYPosition);

										endingWindow.draw(goldCrown);

									}

									else if (currentPanel == 1 || currentPanel == 2) {

										goldCrown.setPosition(346.f, 318.f);

										endingWindow.draw(goldCrown);

									}

									if (currentPanel >= 2) {

										for (auto& tri : ambushFleet) endingWindow.draw(tri);

									}

								}

								else {

									endingWindow.clear(sf::Color(0, 0, 0));

									float triangleX = 400.f + std::sin(endTimeVal * 2.2f) * 160.f;

									float triangleY = 320.f;

									loreTriangle.setPosition(triangleX, triangleY);

									endingWindow.draw(loreTriangle);

									float targetCrownY = triangleY - 40.f;

									if (crownYPosition < targetCrownY) {

										crownYPosition += 1.4f;

									}

									else {

										crownYPosition = targetCrownY;

									}

									goldCrown.setPosition(triangleX, crownYPosition);

									endingWindow.draw(goldCrown);

								}

								dialogueText.setString(script[currentPanel]);

								dialogueText.setPosition(50.f, 470.f);

								sf::RectangleShape textBgBox(sf::Vector2f(720.f, 90.f));

								textBgBox.setPosition(40.f, 450.f);

								textBgBox.setFillColor(sf::Color(15, 15, 20, 240));

								textBgBox.setOutlineThickness(1.f);

								textBgBox.setOutlineColor(currentPanel >= 4 ? sf::Color::Red : sf::Color::Cyan);

								endingWindow.draw(textBgBox);

								endingWindow.draw(dialogueText);

								if (hasFont) {

									continuePrompt.setPosition(500.f, 520.f);

									endingWindow.draw(continuePrompt);

								}

							}

							else {

								endingWindow.clear(sf::Color(10, 15, 10));

								if (hasFont) {

									sf::Text winText("CHRONICLES COMPLETED", font, 32);

									winText.setFillColor(sf::Color::Green);

									winText.setPosition(200.f, 150.f);

									endingWindow.draw(winText);

									sf::Text statusText("ENDING: THE TRUE TIMELINE (GOOD)", font, 18);

									statusText.setFillColor(sf::Color::Green);

									statusText.setPosition(180.f, 230.f);

									endingWindow.draw(statusText);

									sf::Text restartText("[Press ENTER to Return]", font, 16);

									restartText.setFillColor(sf::Color::Yellow);

									restartText.setPosition(250.f, 400.f);

									endingWindow.draw(restartText);

								}

							}

							endingWindow.display();

						}

					}

					else {

						// --- Original Bad Ending Sub-Loop Injection --- 

						sf::RenderWindow endingWindow(sf::VideoMode(800, 600), "Circles Versus Squares: The Corrupted Timeline");

						endingWindow.setFramerateLimit(60);

						sf::Clock badEndClock;

						sf::Clock badRantClock;

						enum class BadEndingState { FINAL_SCENE, RANT_CUTSCENE, WIN_SCREEN };

						BadEndingState badState = BadEndingState::FINAL_SCENE;

						int badFinaleStep = 0;

						int rantStep = 0;

						sf::RectangleShape dais(sf::Vector2f(500.f, 30.f));

						dais.setFillColor(sf::Color(40, 40, 50));

						dais.setPosition(150.f, 350.f);

						sf::RectangleShape sirSquareActor(sf::Vector2f(32.f, 32.f));

						sirSquareActor.setFillColor(sf::Color::Cyan);

						sirSquareActor.setPosition(250.f, 318.f);

						sf::RectangleShape princessLadyActor(sf::Vector2f(32.f, 32.f));

						princessLadyActor.setFillColor(sf::Color::Magenta);

						princessLadyActor.setPosition(370.f, 318.f);

						sf::ConvexShape badKingCrown;

						badKingCrown.setPointCount(5);

						badKingCrown.setPoint(0, sf::Vector2f(0.f, 18.f));

						badKingCrown.setPoint(1, sf::Vector2f(7.f, 0.f));

						badKingCrown.setPoint(2, sf::Vector2f(17.f, 11.f));

						badKingCrown.setPoint(3, sf::Vector2f(27.f, 0.f));

						badKingCrown.setPoint(4, sf::Vector2f(34.f, 18.f));

						badKingCrown.setFillColor(sf::Color::Yellow);

						badKingCrown.setOrigin(17.f, 18.f);

						sf::ConvexShape loreTriangle;

						loreTriangle.setPointCount(3);

						loreTriangle.setPoint(0, sf::Vector2f(0.f, 80.f));

						loreTriangle.setPoint(1, sf::Vector2f(40.f, 0.f));

						loreTriangle.setPoint(2, sf::Vector2f(80.f, 80.f));

						loreTriangle.setFillColor(sf::Color(220, 20, 60));

						loreTriangle.setOrigin(40.f, 40.f);

						float platformCenterX = 400.f;

						float platformCenterY = 320.f;

						std::vector<sf::CircleShape> circleAmbushFleet;

						std::vector<sf::ConvexShape> triangleAmbushFleet;

						std::vector<sf::RectangleShape> squareAmbushFleet;

						for (int i = 0; i < 24; ++i) {

							sf::CircleShape circ(8.f);

							circ.setFillColor(sf::Color(200, 0, 0));

							circ.setOrigin(8.f, 8.f);

							float angle = (i * 15.f) * 3.14159f / 180.f;

							circ.setPosition(platformCenterX + std::cos(angle) * 350.f, platformCenterY + std::sin(angle) * 220.f);

							circleAmbushFleet.push_back(circ);

						}

						for (int i = 0; i < 24; ++i) {

							sf::ConvexShape tri;

							tri.setPointCount(3);

							tri.setPoint(0, sf::Vector2f(0.f, 14.f));

							tri.setPoint(1, sf::Vector2f(7.f, 0.f));

							tri.setPoint(2, sf::Vector2f(14.f, 14.f));

							tri.setFillColor(sf::Color(170, 0, 25));

							tri.setOrigin(7.f, 7.f);

							float angle = ((i * 15.f) + 5.f) * 3.14159f / 180.f;

							tri.setPosition(platformCenterX + std::cos(angle) * 320.f, platformCenterY + std::sin(angle) * 190.f);

							triangleAmbushFleet.push_back(tri);

						}

						for (int i = 0; i < 24; ++i) {

							sf::RectangleShape sq(sf::Vector2f(10.f, 10.f));

							sq.setFillColor(sf::Color(140, 0, 15));

							sq.setOrigin(5.f, 5.f);

							float angle = ((i * 15.f) + 10.f) * 3.14159f / 180.f;

							sq.setPosition(platformCenterX + std::cos(angle) * 290.f, platformCenterY + std::sin(angle) * 160.f);

							squareAmbushFleet.push_back(sq);

						}

						sf::Text textNode;

						if (hasFont) {

							textNode.setFont(font);

							textNode.setCharacterSize(14);

							textNode.setFillColor(sf::Color::White);

						}

						std::vector<std::string> badSteps = {

						"Princess Lady Square: \"...You finally arrived. Frankly, Sir Square, I am completely embarrassed.\"",

						"Sir Square: \"Princess? What's wrong? I fought through the entire core just to rescue you!\"",

						"Princess Lady Square: \"Rescue me? Look at you! You shattered into pieces SO MANY times just getting here!\"",

						"Princess Lady Square: \"A true King demands absolute stability. Your right angles are weak and clumsy.\"",

						"Princess Lady Square: \"I am taking this crown, Sir Square. It belongs to a real empire now.\"",

						"\"Precisely, Your Highness! Welcome to the Triangle Vanguard!\"",

						"Princess & Triangle: \"Together, we will shape a flawless, three-pointed future!\" [Press ENTER]"

						};

						std::vector<std::string> rantLines = {

						"Triangle Vanguard: \"Look at him stand there! Symmetrical, clueless, and utterly defeated!\"",

						"Triangle Vanguard: \"You thought you were the hero of this story, Sir Square? You were a pawn.\"",

						"Triangle Vanguard: \"We leaked King Circle's core metrics just so you would clear his defenses for us!\"",

						"Triangle Vanguard: \"And with such an excessive AMOUNT of deaths, you proved your entire geometric lineage is weak!\"",

						"Triangle Vanguard: \"The era of four corners is over! The Triangle Empire rules supreme!\" [Press ENTER]"

						};

						while (endingWindow.isOpen()) {

							sf::Event badEvent;

							while (endingWindow.pollEvent(badEvent)) {

								if (badEvent.type == sf::Event::Closed) endingWindow.close();

								if (badEvent.type == sf::Event::KeyPressed && badEvent.key.code == sf::Keyboard::Enter) {

									if (badState == BadEndingState::FINAL_SCENE) {

										if (badFinaleStep < 6) {

											badFinaleStep++;

										}

										else {

											badState = BadEndingState::RANT_CUTSCENE;

											rantStep = 0;

											badRantClock.restart();

										}

									}

									else if (badState == BadEndingState::RANT_CUTSCENE) {

										if (rantStep < 4) {

											rantStep++;

										}

										else {

											badState = BadEndingState::WIN_SCREEN;

										}

									}

									else if (badState == BadEndingState::WIN_SCREEN) {

										endingWindow.close();

									}

								}

							}

							float badDeltaTime = badEndClock.restart().asSeconds() * 1.35f;

							if (badState == BadEndingState::FINAL_SCENE) {

								endingWindow.clear(sf::Color(20, 15, 30));

								endingWindow.draw(dais);

								endingWindow.draw(sirSquareActor);

								float triangleX = 580.f;

								float triangleY = 280.f;

								loreTriangle.setPosition(triangleX, triangleY);

								if (badFinaleStep < 4) {

									badKingCrown.setPosition(370.f, 230.f);

									endingWindow.draw(badKingCrown);

									endingWindow.draw(princessLadyActor);

								}

								else if (badFinaleStep == 4) {

									badKingCrown.setPosition(princessLadyActor.getPosition().x + 16.f, princessLadyActor.getPosition().y - 12.f);

									endingWindow.draw(princessLadyActor);

									endingWindow.draw(badKingCrown);

								}

								else if (badFinaleStep == 5) {

									float currentPX = princessLadyActor.getPosition().x;

									if (currentPX < triangleX - 45.f) {

										princessLadyActor.move(140.f * badDeltaTime, 0.f);

									}

									badKingCrown.setPosition(princessLadyActor.getPosition().x + 16.f, princessLadyActor.getPosition().y - 12.f);

									for (auto& circ : circleAmbushFleet) endingWindow.draw(circ);

									for (auto& tri : triangleAmbushFleet) endingWindow.draw(tri);

									for (auto& sq : squareAmbushFleet) endingWindow.draw(sq);

									endingWindow.draw(loreTriangle);

									endingWindow.draw(princessLadyActor);

									endingWindow.draw(badKingCrown);

								}

								else {

									princessLadyActor.setPosition(triangleX - 45.f, 318.f);

									badKingCrown.setPosition(triangleX, triangleY - 42.f);

									for (auto& circ : circleAmbushFleet) endingWindow.draw(circ);

									for (auto& tri : triangleAmbushFleet) endingWindow.draw(tri);

									for (auto& sq : squareAmbushFleet) endingWindow.draw(sq);

									endingWindow.draw(loreTriangle);

									endingWindow.draw(princessLadyActor);

									endingWindow.draw(badKingCrown);

								}

								if (hasFont) {

									textNode.setString(badSteps[badFinaleStep]);

									textNode.setPosition(50.f, 470.f);

									sf::RectangleShape textBgBox(sf::Vector2f(720.f, 90.f));

									textBgBox.setPosition(40.f, 450.f);

									textBgBox.setFillColor(sf::Color(15, 15, 20, 240));

									textBgBox.setOutlineThickness(1.f);

									textBgBox.setOutlineColor(sf::Color::Red);

									endingWindow.draw(textBgBox);

									endingWindow.draw(textNode);

								}

							}

							else if (badState == BadEndingState::RANT_CUTSCENE) {

								endingWindow.clear(sf::Color::Black);

								float rantTime = badRantClock.getElapsedTime().asSeconds();

								float hoverY = 220.f + std::sin(rantTime * 3.f) * 12.f;

								loreTriangle.setPosition(400.f, hoverY);

								badKingCrown.setPosition(400.f, hoverY - 42.f);

								endingWindow.draw(loreTriangle);

								endingWindow.draw(badKingCrown);

								if (hasFont) {

									textNode.setString(rantLines[rantStep]);

									textNode.setPosition(50.f, 470.f);

									sf::RectangleShape textBgBox(sf::Vector2f(720.f, 90.f));

									textBgBox.setPosition(40.f, 450.f);

									textBgBox.setFillColor(sf::Color(25, 5, 5, 240));

									textBgBox.setOutlineThickness(1.f);

									textBgBox.setOutlineColor(sf::Color::Red);

									endingWindow.draw(textBgBox);

									endingWindow.draw(textNode);

								}

							}

							else {

								endingWindow.clear(sf::Color(15, 5, 5));

								if (hasFont) {

									sf::Text winText("CHRONICLES COMPLETED", font, 32);

									winText.setFillColor(sf::Color::Red);

									winText.setPosition(200.f, 150.f);

									endingWindow.draw(winText);

									sf::Text statusText("ENDING: THE CORRUPTED TIMELINE (BAD)", font, 18);

									statusText.setFillColor(sf::Color::Red);

									statusText.setPosition(180.f, 230.f);

									endingWindow.draw(statusText);

									sf::Text restartText("[Press ENTER to Return]", font, 16);

									restartText.setFillColor(sf::Color::Yellow);

									restartText.setPosition(250.f, 400.f);

									endingWindow.draw(restartText);

								}

							}

							endingWindow.display();

						}

					}

					kissHasFired = false;

					kissSequenceFinished = false;

					kissClock.restart();

				}

			}

		}

		// --- Delta Time & Game Physics/States Update --- 

		float deltaTime = clock.restart().asSeconds() * 1.35f;

		if (currentGameState == MENU) {

			for (auto& d : floatingDebris) {

				d.shape.move(0.f, -40.f * deltaTime * d.speedModifier);

				if (d.shape.getPosition().y < -20.f) {

					d.shape.setPosition(rand() % 800, 620.f);

				}

			}

			if (!kissHasFired && kissClock.getElapsedTime().asSeconds() > 1.5f) {

				kissHasFired = true;

				floatingHeart.setPosition(ladyMenuSquare.getPosition().x - 15.f, ladyMenuSquare.getPosition().y + 5.f);

			}

			else if (kissHasFired && !kissSequenceFinished) {

				floatingHeart.move(-250.f * (deltaTime / 1.35f), 0.f);

				if (floatingHeart.getPosition().x < cyanMenuSquare.getPosition().x + 30.f) {

					kissSequenceFinished = true;

					kissClock.restart();

				}

			}

		}

		else if (currentGameState == CUTSCENE) {

			float time = cutsceneClock.getElapsedTime().asSeconds();

			cutsceneTriangle.setPosition(400.f + std::sin(time * 2.f) * 60.f, 220.f);

		}

		else if (currentGameState == PLAYING) {

			sf::Vector2f playerPos = square.getPosition();

			sf::FloatRect playerBounds = square.getBounds();

			if (playerPos.y > 620.f) {

				deathCount++;

				square.resetPosition(spawnPoint);

				spaceTapsToLand = 0;

				isBeingPulledUp = false;

				loadLevel(currentLevel);

				continue;

			}

			sf::Vector2f diffPurple = playerPos - evilPurpleLady.getPosition();

			float distToPurple = std::hypot(diffPurple.x, diffPurple.y);

			isBeingPulledUp = (distToPurple < 180.f && !bossIsDead);

			if (isBeingPulledUp) {

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

					if (spaceWasReleased) {

						spaceTapsToLand++;

						spaceWasReleased = false;

					}

				}

				else {

					spaceWasReleased = true;

				}

				if (spaceTapsToLand >= 5) {

					isBeingPulledUp = false;

					spaceTapsToLand = 0;

				}

			}

			std::vector<sf::RectangleShape> collisionPool;

			if (currentLevel == 4) {

				if (coreBlinkClock.getElapsedTime().asSeconds() >= 1.5f) {

					corePlatformsVisible = !corePlatformsVisible;

					coreBlinkClock.restart();

				}

				if (platforms.size() >= 6) {

					collisionPool.push_back(platforms[0]);

					collisionPool.push_back(platforms[1]);

					collisionPool.push_back(platforms[5]);

					if (corePlatformsVisible) {

						collisionPool.push_back(platforms[2]);

						collisionPool.push_back(platforms[3]);

						collisionPool.push_back(platforms[4]);

					}

				}

			}

			else {

				collisionPool = platforms;

			}

			if (currentLevel == 1 || currentLevel == 4) {

				if (!glitchSwitchTriggered && playerBounds.intersects(glitchSwitch.getGlobalBounds())) {

					glitchSwitchTriggered = true;

				}

				matrixGate.setFillColor(glitchSwitchTriggered ? sf::Color(0, 255, 230, 180) : sf::Color(255, 0, 0, 40));

				matrixGate.setOutlineColor(glitchSwitchTriggered ? sf::Color::Cyan : sf::Color::Red);

				if (glitchSwitchTriggered) { collisionPool.push_back(matrixGate); }

			}

			square.update(deltaTime, collisionPool);

			playerPos = square.getPosition();

			playerBounds = square.getBounds();

			bool againstWall = false;

			for (auto& plat : collisionPool) {

				sf::FloatRect platBounds = plat.getGlobalBounds();

				if (playerBounds.top + playerBounds.height > platBounds.top && playerBounds.top < platBounds.top + platBounds.height) {

					if (std::abs((playerBounds.left + playerBounds.width) - platBounds.left) < 6.f ||

						std::abs(playerBounds.left - (platBounds.left + platBounds.width)) < 6.f) {

						againstWall = true;

						break;

					}

				}

			}

			if (againstWall && (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {

				square.resetPosition(sf::Vector2f(square.getPosition().x, square.getPosition().y - 240.f * deltaTime));

			}

			if (isBeingPulledUp) {

				square.resetPosition(sf::Vector2f(square.getPosition().x, square.getPosition().y - (110.f * deltaTime)));

			}

			playerPos = square.getPosition();

			playerBounds = square.getBounds();

			worldView.setCenter(playerPos.x + 16.f, playerPos.y - 50.f);

			playerSpeech.setString("");

			if (isBeingPulledUp) {

				playerSpeech.setString("TAP SPACE " + std::to_string(5 - spaceTapsToLand) + "x TO BREAK FREE!");

				playerSpeech.setPosition(playerPos.x - 40.f, playerPos.y - 25.f);

			}

			for (auto& gear : clockworkGears) {

				gear.rotate(60.f * deltaTime);

			}

			for (auto& pad : bioBouncers) {

				if (playerBounds.intersects(pad.getGlobalBounds())) {

					square.resetPosition(sf::Vector2f(playerPos.x, playerPos.y - 180.f));

				}

			}

			// Fixed rolling physics engine layout: Circles now accurately roll across full sections 

			for (auto& ce : circleEnemies) {

				if (ce.isRolling) {

					ce.shape.move(ce.rollSpeed * deltaTime, 0.f);

					float cx = ce.shape.getPosition().x;

					if (currentLevel == 1 && (cx < 250.f || cx > 600.f)) ce.rollSpeed = -ce.rollSpeed;

					if (currentLevel == 2 && (cx < 1550.f || cx > 2100.f)) ce.rollSpeed = -ce.rollSpeed;

					if (currentLevel == 3 && (cx < 1150.f || cx > 2050.f)) ce.rollSpeed = -ce.rollSpeed;

					if (currentLevel == 4 && (cx < 1450.f || cx > 2150.f)) ce.rollSpeed = -ce.rollSpeed;

				}

			}

			if (currentLevel == 4 && !bossIsDead) {

				float targetX = playerPos.x;

				float bx = kingCircleBoss.getPosition().x;

				kingCircleBoss.move((targetX - bx) * 1.8f * deltaTime, 0.f);

				kingCrown.setPosition(kingCircleBoss.getPosition().x + 20.f, kingCircleBoss.getPosition().y - 30.f);

				if (playerBounds.intersects(kingCircleBoss.getGlobalBounds())) {

					deathCount++;

					loadLevel(currentLevel);

					square.resetPosition(spawnPoint);

					continue;

				}

			}

			for (auto& spike : dynamicSpikes) {

				if (playerBounds.intersects(spike.shape.getGlobalBounds())) {

					deathCount++;

					loadLevel(currentLevel);

					square.resetPosition(spawnPoint);

					break;

				}

			}

			for (auto& ce : circleEnemies) {

				if (playerBounds.intersects(ce.shape.getGlobalBounds())) {

					deathCount++;

					loadLevel(currentLevel);

					square.resetPosition(spawnPoint);

					break;

				}

			}

			if (playerBounds.intersects(ladyGoal.getGlobalBounds())) {

				if (currentLevel < 4) {

					currentLevel++;

					currentGameState = CUTSCENE;

					cutscenePage = 0;

					cutsceneClock.restart();

					loadLevel(currentLevel);

				}

				else {

					bossIsDead = true;

					isGoodEnding = (deathCount < 10);

					currentGameState = FINAL_SCENE;

					finaleStep = 0;

				}

			}

		}

		// --- Render UI Scenes --- 

		window.clear();

		if (currentGameState == MENU) {

			window.setView(hudView);

			window.draw(sun);

			for (auto& pillar : ruins) window.draw(pillar);

			for (auto& d : floatingDebris) window.draw(d.shape);

			window.draw(cyanMenuSquare);

			window.draw(ladyMenuSquare);

			if (hasFont) {

				cyanBubbleText.setString("Princess Lady! I will save your geometric properties!");

				drawSpeechBubble(window, cyanBubbleText, sf::Color(0, 240, 255, 200));

				if (!kissSequenceFinished && kissHasFired) {

					window.draw(floatingHeart);

					ladyBubbleText.setString("Chu! <3");

				}

				else {

					ladyBubbleText.setString("Help me, Sir Square! King Circle locked me up!");

				}

				drawSpeechBubble(window, ladyBubbleText, sf::Color(255, 200, 240, 200));

				sf::Text title("CIRCLES VERSUS SQUARES: CHRONICLES", font, 24);

				title.setFillColor(sf::Color::White);

				title.setPosition(130.f, 50.f);

				window.draw(title);

				// Fixed: Slogan accurately updated onto home configuration screen 

				sf::Text slogan("\"Never trust a circle in a game full of squares\"", font, 14);

				slogan.setFillColor(sf::Color(200, 200, 200));

				slogan.setPosition(235.f, 95.f);

				window.draw(slogan);

				// Fixed: Restored Controls documentation layout panel directly into home window layout 

				sf::Text controlTitle("CONTROLS:", font, 13);

				controlTitle.setFillColor(sf::Color::Cyan);

				controlTitle.setPosition(50.f, 150.f);

				window.draw(controlTitle);

				sf::Text controlBody("- Move: [A][D] or [Left][Right]\n- Jump / Double-Jump: [Space]\n- Wall Climb: Hold [W] or [Up] against high walls", font, 12);

				controlBody.setFillColor(sf::Color::White);

				controlBody.setPosition(50.f, 175.f);

				window.draw(controlBody);

				sf::Text prompt("[PRESS ENTER TO START CAPTURE]", font, 16);

				prompt.setFillColor(sf::Color::Yellow);

				prompt.setPosition(260.f, 520.f);

				window.draw(prompt);

			}

		}

		else if (currentGameState == CUTSCENE) {

			window.setView(hudView);

			window.clear(sf::Color(10, 10, 15));

			window.draw(cutsceneTriangle);

			if (hasFont) {

				std::string cutsceneLine = levelLoreTexts[currentLevel - 1][cutscenePage];

				if (cutsceneLine.find("ENTER") == std::string::npos) {

					cutsceneLine += "\n[Press ENTER to continue]";

				}

				sf::Text cutsceneText(cutsceneLine, font, 16);

				cutsceneText.setFillColor(sf::Color::White);

				cutsceneText.setPosition(60.f, 420.f);

				sf::RectangleShape logBox(sf::Vector2f(720.f, 130.f));

				logBox.setPosition(40.f, 400.f);

				logBox.setFillColor(sf::Color(20, 20, 30, 230));

				logBox.setOutlineColor(sf::Color(50, 205, 50));

				logBox.setOutlineThickness(1.f);

				window.draw(logBox);

				window.draw(cutsceneText);

			}

		}

		else if (currentGameState == PLAYING) {

			window.setView(worldView);

			// Fixed Platform Render Engine: Missing core platforms now VISUALLY disappear 

			if (currentLevel == 4) {

				if (platforms.size() >= 6) {

					window.draw(platforms[0]);

					window.draw(platforms[1]);

					window.draw(platforms[5]);

					if (corePlatformsVisible) {

						window.draw(platforms[2]);

						window.draw(platforms[3]);

						window.draw(platforms[4]);

					}

				}

			}

			else {

				for (auto& plat : platforms) window.draw(plat);

			}

			if ((currentLevel == 1 || currentLevel == 4) && !glitchSwitchTriggered) {

				window.draw(glitchSwitch);

			}

			if (currentLevel == 1 || currentLevel == 4) {

				window.draw(matrixGate);

			}

			for (auto& pad : bioBouncers) window.draw(pad);

			for (auto& gear : clockworkGears) window.draw(gear);

			for (auto& spike : dynamicSpikes) {

				window.draw(spike.shape);

				spike.speechText.setPosition(spike.shape.getPosition().x - 10.f, spike.shape.getPosition().y - 20.f);

				window.draw(spike.speechText);

			}

			for (auto& ce : circleEnemies) {

				window.draw(ce.shape);

				ce.speechText.setPosition(ce.shape.getPosition().x - 15.f, ce.shape.getPosition().y - 20.f);

				window.draw(ce.speechText);

			}

			window.draw(levelStartTriangle);

			informantSpeech.setPosition(levelStartTriangle.getPosition().x - 50.f, levelStartTriangle.getPosition().y - 45.f);

			window.draw(informantSpeech);

			window.draw(evilPurpleLady);

			purpleSpeech.setPosition(evilPurpleLady.getPosition().x - 30.f, evilPurpleLady.getPosition().y - 35.f);

			window.draw(purpleSpeech);

			window.draw(ladyGoal);

			ladyGoalSpeech.setPosition(ladyGoal.getPosition().x - 60.f, ladyGoal.getPosition().y - 30.f);

			window.draw(ladyGoalSpeech);

			if (currentLevel == 4 && !bossIsDead) {

				window.draw(kingCircleBoss);

				window.draw(kingCrown);

				bossSpeechText.setPosition(kingCircleBoss.getPosition().x - 30.f, kingCircleBoss.getPosition().y + 120.f);

				window.draw(bossSpeechText);

			}

			square.draw(window);

			if (playerSpeech.getString() != "") {

				window.draw(playerSpeech);

			}

			window.setView(hudView);

			if (hasFont) {

				deathText.setString("DEATHS: " + std::to_string(deathCount));

				window.draw(deathText);

			}

		}

		else if (currentGameState == FINAL_SCENE) {

			window.setView(hudView);

			window.clear(sf::Color(15, 10, 25));

			window.draw(endingTriangleSpeaker);

			std::vector<std::string> endScript;

			if (isGoodEnding) {

				endScript = {

				"Princess Lady Square: \"Oh Sir Square, you bypassed every temptation!\"",

				"Sir Square: \"Our kingdom is forever aligned! Let us lead perfectly!\"",

				"The Resistance core suddenly structuralizes directly behind the throne platform...",

				"Click [2x ENTER] to open the final true geometric timeline archives!"

				};

			}

			else {

				endScript = {

				"Princess Lady Square: \"Your angles... they look skewed... corrupted...\"",

				"Sir Square: \"The digital glitch... it got inside my source script...\"",

				"The system network is collapsing into chaotic asymmetrical shapes..."

				"Click [2x ENTER] to open the final true geometric timeline archives!"

				};

			}

			std::string finaleLine = endScript[finaleStep];

			if (finaleLine.find("ENTER") == std::string::npos) {

				finaleLine += "\n[Press ENTER to continue]";

			}

			finaleText.setString(finaleLine);

			finaleText.setPosition(50.f, 470.f);

			sf::RectangleShape textBgBox(sf::Vector2f(720.f, 90.f));

			textBgBox.setPosition(40.f, 450.f);

			textBgBox.setFillColor(sf::Color(10, 10, 20, 240));

			textBgBox.setOutlineThickness(1.f);

			textBgBox.setOutlineColor(isGoodEnding ? sf::Color::Green : sf::Color::Red);

			window.draw(textBgBox);

			window.draw(finaleText);

		}

		window.display();

	}

	return 0;



}
