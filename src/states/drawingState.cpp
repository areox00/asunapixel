#include "drawingState.hpp"
#include "app.hpp"

DrawingState::DrawingState(App *app)
	: State(app)
{
	viewport.init(&app->window);
	canvas.setStrokeColor(sf::Color(0xFF, 0x83, 0x60));
}

DrawingState::~DrawingState()
{

}

void DrawingState::processGui()
{
	sf::View guiView;
	guiView.setCenter((sf::Vector2f)app->window.getSize() / 2.f);
	guiView.setSize((sf::Vector2f)app->window.getSize());
	app->window.setView(guiView);

	sf::Vector2f guiPos = app->window.mapPixelToCoords(app->pixelPos);

	gui.begin(app->window.getSize(), Layout::VERTICAL);

	// bar
	gui.pushBox({gui.getSize().x, 32}, Layout::HORIZONTAL);
	gui.fill(app->window, sf::Color(0x16, 0x20, 0x2D));
	gui.padding({32.f , 0.0});

	// block drawing when mouse enters the bar
	if (gui.block(guiPos))
		canPaint = false;
	else
		canPaint = true;

	// basic colors for testing gui
	std::vector<sf::Color> upperColors = {
		sf::Color(0xFF, 0x83, 0x60),
		sf::Color(0xE8, 0xE2, 0x88),
		sf::Color(0x7D, 0xCE, 0x82)
	};

	std::vector<sf::Color> bottomColors = {
		sf::Color(0x3C, 0xDB, 0xD3),
		sf::Color(0x42, 0xB8, 0xF6),
		sf::Color(0xA6, 0xA2, 0xF9)
	};

	// palette
	gui.pushBox({gui.getSize().y / 2 * 3, gui.getSize().y}, Layout::HORIZONTAL);
	gui.padding({6.f, 6.f});

	sf::Color fillColor;

	// upper row
	gui.pushBox({gui.getSize().x, gui.getSize().y / 2}, Layout::VERTICAL);
	for (const auto &color : upperColors) {
		fillColor = color;
		gui.pushBox({gui.getSize().y, gui.getSize().y}, Layout::HORIZONTAL);

		if (gui.hover(guiPos)) {
			fillColor.a = 150;
		}

		if (gui.pressed(guiPos)) {
			canvas.setStrokeColor(color);
			fillColor.a = 100;
		}

		gui.fill(app->window, fillColor);
		gui.popBox();
	}
	gui.popBox();

	// bottom row
	gui.pushBox({gui.getSize().x, gui.getRemainingSize().y}, Layout::VERTICAL);
	for (const auto &color : bottomColors) {
		fillColor = color;
		gui.pushBox({gui.getSize().y, gui.getSize().y}, Layout::HORIZONTAL);

		if (gui.hover(guiPos)) {
			fillColor.a = 150;
		}

		if (gui.pressed(guiPos)) {
			canvas.setStrokeColor(color);
			fillColor.a = 100;
		}

		gui.fill(app->window, fillColor);
		gui.popBox();
	}
	gui.popBox();
	// pop palette box and return to panel box
	gui.popBox();

	// color preview
	gui.pushBox({gui.getSize().y, gui.getSize().y}, Layout::HORIZONTAL);
	gui.padding(gui.getSize() / 2.f);
	gui.fill(app->window, canvas.getStrokeColor());
	gui.popBox();

	gui.space(8.f);

	// text
	sf::Font font;
	font.loadFromFile("res/fonts/UbuntuMono-Bold.ttf");

	gui.pushBox({100, gui.getSize().y / 2.f}, Layout::VERTICAL);
	gui.padding({0, 6.f});
	gui.text(app->window, 12, "brush size: " + std::to_string(int(strokeSize * 63) + 1), font);
	gui.popBox();

	// slider
	gui.pushBox({100, gui.getRemainingSize().y}, Layout::HORIZONTAL);
	gui.padding({0, 6.f});
	gui.slider(app->window, guiPos, strokeSize);
	canvas.setStrokeSize(strokeSize * 63 + 1);
}

void DrawingState::processEvent(sf::Event &event)
{
	viewport.processEvent(event);

	switch (event.type) {
		case sf::Event::Closed:
			app->window.close();
			break;
		case sf::Event::LostFocus:
			canPaint = false;
			break;
		case sf::Event::GainedFocus:
			canPaint = true;
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Equal)
				gui.scale(1.1);
			if (event.key.code == sf::Keyboard::Hyphen)
				gui.scale(0.9);
			break;
		default:
			break;
	}
}

void DrawingState::update()
{
	canvas.update(app->worldPos, viewport.getBounds(), canPaint);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Add))
		gui.scale(1.1);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Subtract))
		gui.scale(0.9);
}

void DrawingState::draw(float alpha)
{
	app->window.setView(viewport.getView());
	canvas.draw(app->window);

	processGui();

	// return back to world view
	app->window.setView(viewport.getView());
}