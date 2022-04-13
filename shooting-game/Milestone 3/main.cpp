#include <iostream>
#include "SDL.h"
#include "SDL_image.h"
#include <vector>
#include <bitset>

#define _DEBUG3_

enum class rect_side;
bool action_reduce_Players_HP = false;

const int reduce_hp_on_obstacles_wall_collision = 5;

class Body;
class BodyManager;
class Interval;

// SDL related
SDL_Window* gp_window = nullptr;
SDL_Renderer* gp_renderer = nullptr;
SDL_Event		g_event;
const SDL_Rect	g_window_rect = { 0,0,800,640 };

// for game
std::shared_ptr<Body> gp_player = nullptr;
Uint32 g_kill_count = 0; // related to score, game_level
Uint32 g_bullet_count = 0; // total bullet created
Uint32 g_game_level = 1;


void check_collisions();
void check_events();
void display_status();
void game_level_update();
void game_update();

// for collision check
enum class rect_side {
	top, left, bottom, right
};


enum class action_on_collision {
	reset_health_power, reset_hp_and_reduce_players_hp
};

class Interval {
public:
	Interval(Uint32 _interval = 16) {
		interval = _interval;
		last_time = SDL_GetTicks() + interval;
	}

	bool is_good() {
		Uint32 now = SDL_GetTicks();
		if ((now - last_time) < (interval)) return false;
		last_time = now;
		return true;
	}
	void set_interval(const Uint32& _interval) {
		interval = _interval;
		last_time = SDL_GetTicks() + interval;
	}
	//private:
	Uint32 interval = 10;
	Uint32 last_time = SDL_GetTicks() + interval;
};


class Body {
public:
	SDL_Rect from{ 0,0,0,0 }; // Texture coordinator
	SDL_Rect to{ 0,0,0,0 }; // screen coordinator
	int health_power = 10;
	int attack_power = 10;
};

class ScrollableBody {
public:
	SDL_Rect from1{ 0,0,0,0 }; // Texture coordinator
	SDL_Rect to1{ 0,0,0,0 }; // screen coordinator

	SDL_Rect from2{ 0,0,0,0 }; // Texture coordinator
	SDL_Rect to2{ 0,0,0,0 }; // screen coordinator

	SDL_Rect viewport{ 0,0,0,0 }; // screen coordinator
};

class ScrollableBodyManager {
public:
	ScrollableBodyManager(const std::string& filename, int speed = 0) {
		image_path = filename;
		this->speed = speed;
	}
	SDL_Texture* get_texture() {
		return p_texture;
	}
	bool init(SDL_Renderer* pRenderer) {
		p_renderer = pRenderer;
		SDL_Surface* pSurf = IMG_Load(image_path.c_str());
		if (pSurf == NULL) {
			std::cout << "ErrorIMG_Load(image_filename.c_str());" << std::endl;
			return false;
		}
		p_texture = SDL_CreateTextureFromSurface(p_renderer, pSurf);
		if (p_texture == NULL) {
			// error
			std::cout << "Error SDL_CreateTextureFromSurface(...)::" << image_path << std::endl;
			return false;
		}
		SDL_FreeSurface(pSurf);

		SDL_QueryTexture(p_texture, NULL, NULL, &texture_w, &texture_h);
		return true;
	}
	std::shared_ptr<ScrollableBody> create(const SDL_Rect& _viewport) {
		std::shared_ptr<ScrollableBody> body = std::make_shared<ScrollableBody>();
		body->viewport = _viewport;
		body->from1 = _viewport;
		body->to1 = _viewport;

		body->from2 = _viewport;
		body->from2.h = 0;
		body->to2 = _viewport;;
		body->to2.h = 0;

		vp_bodies.emplace_back(body);
		return body;
	}
	void set_speed(int speed) {
		this->speed = speed;
		if (this->speed < 1) this->speed = 1;
	}
	void increase_speed() {
		speed++;
	}
	void decrease_speed() {
		speed--;
		if (speed < 1) speed = 1;
	}

	void move_up(bool keep_going = false) {
		for (auto body : vp_bodies) {

			body->from1.y += speed;
			body->from1.y = body->from1.y % texture_h; // loop 0 - texture_h
			if ((texture_h - body->from1.y) < body->viewport.h) {
				body->from1.h = texture_h - body->from1.y;
			}
			body->to1.h = body->from1.h;
			body->to1.y = body->viewport.y;

			body->from2.h = body->viewport.h - body->from1.h;
			body->from2.y = body->from1.y + body->from1.h;
			body->from2.y = body->from2.y % texture_h; // loop 0 - texture_h

			body->to2.h = body->from2.h;
			body->to2.y = body->to1.y + body->to1.h;

#ifdef _DEBUG2_
			std::cout << "(F1.y: " << body->from1.y <<
				",F1.h: " << body->from1.h <<
				",T1.y: " << body->to1.y <<
				",T1.h: " << body->to1.h <<
				",F2.y: " << body->from2.y <<
				",F2.h: " << body->from2.h <<
				",T2.y: " << body->to2.y <<
				",T2.h: " << body->to2.h <<
				")" << std::endl;
#endif 
		}
	}
	void move_down(bool keep_going = false) {
	}

	void move_left(bool keep_going = false) {
	}

	void move_right(bool keep_going = false) {
	}
	// for background which has scrollable character
	void display() {
		for (auto body : vp_bodies) {
			SDL_RenderCopy(p_renderer, p_texture, &body->from1, &body->to1);
			SDL_RenderCopy(p_renderer, p_texture, &body->from2, &body->to2);
		}
	}

	// Return vector by reference
	std::vector<std::shared_ptr<ScrollableBody> >& get_vp_bodies() {
		return vp_bodies;
	}


private:
	std::vector<std::shared_ptr<ScrollableBody> > vp_bodies;
	SDL_Renderer* p_renderer = nullptr;
	SDL_Texture* p_texture = nullptr;
	int texture_w = 0;
	int texture_h = 0;
	std::string image_path = "";
	int speed = 0;
};



class BodyManager {
public:
	BodyManager(const std::string& filename, int speed = 0) {
		image_path = filename;
		this->speed = speed;
	}
	SDL_Texture* get_texture() {
		return p_texture;
	}
	int get_texture_height() {
		return texture_h;
	}
	int get_texture_width() {
		return texture_w;
	}

	bool init(SDL_Renderer* pRenderer) {
		p_renderer = pRenderer;
		SDL_Surface* pSurf = IMG_Load(image_path.c_str());
		if (pSurf == NULL) {
			std::cout << "ErrorIMG_Load(image_filename.c_str());" << std::endl;
			return false;
		}
		p_texture = SDL_CreateTextureFromSurface(p_renderer, pSurf);
		if (p_texture == NULL) {
			// error
			std::cout << "Error SDL_CreateTextureFromSurface(...)::" << image_path << std::endl;
			return false;
		}
		SDL_FreeSurface(pSurf);
		SDL_QueryTexture(p_texture, NULL, NULL, &texture_w, &texture_h);

		return true;
	}
	std::shared_ptr<Body> create(const SDL_Rect& from, const SDL_Rect& to) {
		std::shared_ptr<Body> body = std::make_shared<Body>();
		body->from = from;
		body->to = to;
		vp_bodies.emplace_back(body);
		return body;
	}
	void display() {
		for (auto body : vp_bodies) {
			SDL_RenderCopy(p_renderer, p_texture, &(body->from), &(body->to));
		}
	}
	void set_speed(int speed) {
		this->speed = speed;
		if (this->speed < 1) this->speed = 1;
	}
	void increase_speed() {
		speed++;
	}
	void decrease_speed() {
		speed--;
		if (speed < 1) speed = 1;
	}

	void move_up(bool keep_going = false) {
		for (auto body : vp_bodies) {
			body->to.y -= speed;
			if (keep_going) continue;
			if (body->to.y <= 0) body->to.y = 0;
		}
	}
	void move_down(bool keep_going = false) {
		for (auto body : vp_bodies) {
			body->to.y += speed;
			if (keep_going) continue;
			if (body->to.y >= (g_window_rect.h - body->to.h)) body->to.y = g_window_rect.h - body->to.h;
		}
	}

	void move_left(bool keep_going = false) {
		for (auto body : vp_bodies) {
			body->to.x -= speed;
			if (keep_going) continue;
			if (body->to.x < 0) body->to.x = 0;
		}
	}

	void move_right(bool keep_going = false) {
		for (auto body : vp_bodies) {
			body->to.x += speed;
			if (keep_going) continue;
			if (body->to.x > (g_window_rect.w - body->to.w)) body->to.x = g_window_rect.w - body->to.w;
		}
	}
	
	// Return vector by reference
	std::vector<std::shared_ptr<Body> >& get_vp_bodies() {
		return vp_bodies;
	}

	Uint32 remove_the_under_healthy() {
		Uint32 count = 0;
		for (auto it = vp_bodies.begin(); it != vp_bodies.end(); ) {
			if ((*it)->health_power <= 0) {
				it = vp_bodies.erase(it);
				count++;
				continue;
			}
			++it;
		}
		return count;
	}

private:
	std::vector<std::shared_ptr<Body> > vp_bodies;
	SDL_Window* p_window = nullptr;
	SDL_Renderer* p_renderer = nullptr;
	SDL_Texture* p_texture = nullptr;
	std::string image_path = "";
	int texture_h = 0;
	int texture_w = 0;
	int speed = 0;
};


// control keyboard input interval
Interval keyin_interval(16);
Interval shooting_interval(128);

ScrollableBodyManager background_manager("assets/images/background/skybackground.png");
BodyManager player_manager("assets/images/player character/player_character.png", 1);
BodyManager enemy_manager("assets/images/enemy/EnemyPlaneV3.png");
BodyManager bullet_manager("assets/images/skill/playerSkill.png", 3);
BodyManager enemy_bullets("assets/images/skill/enemySkillV2.png", 3);

void display_status() {
#ifdef _DEBUG1_
	std::cout << "(E: " <<
		enemy_manager.get_vp_bodies().size() << ", B: " <<
		bullet_manager.get_vp_bodies().size() << ", H: " <<
		gp_player->health_power << ", K: " <<
		g_kill_count << ", TB: " <<
		g_bullet_count << ",G: " <<
		g_game_level << ")" <<
		std::endl;
#endif
#ifdef _DEBUG3_
	std::cout << "(E: " <<
		enemy_manager.get_vp_bodies().size() << ", B: " <<
		enemy_bullets.get_vp_bodies().size() << ", H: " <<
		gp_player->health_power << ", K: " <<
		std::endl;
#endif

}


Interval creating_enemy_interval(2048); //2048 ms
Interval random_shooting_interval(548);

void create_enemy_randomly(BodyManager& em) {

	if (!creating_enemy_interval.is_good()) return;

	int x = rand() % (g_window_rect.w - 64);

	em.create({ 0,0,64,64 }, { x, 0, 64, 64 });
}
void create_shoot_randomly(BodyManager& enemies, BodyManager& enemy_bullets) {

	std::vector<std::shared_ptr<Body>>& vp_enemies = enemies.get_vp_bodies();
	std::vector<std::shared_ptr<Body>>& vp_enemy_bullets = enemy_bullets.get_vp_bodies();

	if (!random_shooting_interval.is_good()) return;

	SDL_Rect from{ 0,0,enemy_bullets.get_texture_width(),enemy_bullets.get_texture_height() };
	SDL_Rect to{ 0,0,from.w,from.h };
	for (auto enemy = vp_enemies.begin(); enemy != vp_enemies.end(); enemy++) {
		to.x = (*enemy)->to.x + 10; //28
		to.y = (*enemy)->to.y + (*enemy)->to.y + 2;
		enemy_bullets.create(from, to);
	}
}

void check_events() {

	while (SDL_PollEvent(&g_event)) {
		if (g_event.type == SDL_QUIT) return;
	}

	// for player's keyboard control

	if (!keyin_interval.is_good()) return;
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

	if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) { player_manager.move_up(); }
	if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) { player_manager.move_left(); }
	if (keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN]) { player_manager.move_down(); }
	if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) { player_manager.move_right(); }
	if (keyboardState[SDL_SCANCODE_SPACE]) {

		if (!shooting_interval.is_good()) return;

		bullet_manager.create({ 0,0,6,32 }, { gp_player->to.x + 29,gp_player->to.y,6,32 });
		g_bullet_count++;

	}
	if (keyboardState[SDL_SCANCODE_KP_PLUS]) { player_manager.increase_speed(); }
	if (keyboardState[SDL_SCANCODE_KP_MINUS]) { player_manager.decrease_speed(); }
}

bool game_init() {
	srand((unsigned int)(time(0)));

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { // Fail
		std::cout << "SDL couldn't initialized!" << std::endl;
		return false;
	}
	std::cout << "Initialized! " << std::endl;

	gp_window = SDL_CreateWindow("Game 000", 100, 100, g_window_rect.w, g_window_rect.h, 0);
	if (!gp_window) {
		std::cout << "Window is not created!" << std::endl;
		return false;
	}
	std::cout << "Windows Created! " << std::endl;

	gp_renderer = SDL_CreateRenderer(gp_window, -1, 0);
	if (!gp_renderer) {
		std::cout << "Renderer is not created!" << std::endl;
		return false;
	}

	// for background color
	SDL_SetRenderDrawColor(gp_renderer, 0, 255, 25, 255);

	// for background image
	if (!background_manager.init(gp_renderer)) return false;
	background_manager.create(g_window_rect);
	background_manager.set_speed(1);

	// for player plane
	if (!player_manager.init(gp_renderer)) return false;
	player_manager.set_speed(2);

	gp_player = player_manager.create({ 0,0,64,64 }, { (g_window_rect.w - 64) / 2,g_window_rect.h - 64, 64, 64 });
	gp_player->health_power = 200;

	if (!enemy_manager.init(gp_renderer)) return false;
	enemy_manager.set_speed(0);

	if (!bullet_manager.init(gp_renderer)) return false;
	bullet_manager.set_speed(7);

	if (!enemy_bullets.init(gp_renderer)) return false;
	enemy_bullets.set_speed(5);
	return true;
}
void game_level_update() {
	Uint32 last_game_level = g_game_level;
	g_game_level = g_kill_count / 10;
	if (g_game_level > last_game_level) {
		creating_enemy_interval.interval -= 20;
	}
}
void game_update() {
	create_enemy_randomly(enemy_manager);
	create_shoot_randomly(enemy_manager, enemy_bullets);

	enemy_manager.move_down(true);

	enemy_bullets.move_down(true);

	bullet_manager.move_up(true);
	background_manager.move_up(true);

	check_collisions();
	check_events();
	game_level_update();
}



rect_side intersect_rect_in_rect(const SDL_Rect& inner, const SDL_Rect& outer) {
	if ((inner.x + inner.w) >= (outer.x + outer.w)) return rect_side::right;
	if ((inner.x) <= (outer.x)) return rect_side::left;
	if ((inner.y) >= (outer.y)) return rect_side::top;
	if ((inner.y + inner.h) >= (outer.y + outer.h)) return rect_side::bottom;
}


rect_side intersect_rect_in_rect_2(const SDL_Rect& inner, const SDL_Rect& outer = g_window_rect, const bool _mode = false) {
	int deltaX = 0;
	int deltaY = 0;
	if (true) deltaX = inner.w;
	if (true) deltaY = inner.h;
	if ((inner.x + deltaX) >= (outer.x + outer.w)) return rect_side::right;
	if ((inner.x) <= (outer.x)) return rect_side::left;
	if ((inner.y) >= (outer.y)) return rect_side::top;
	if ((inner.y + deltaY) >= (outer.y + outer.h)) return rect_side::bottom;
}

void on_collision_to_window_wall(std::vector<std::shared_ptr<Body>>& vp_objects,
	rect_side _side = rect_side::bottom,
	bool is_reduce_players_HP = false) {

	for (auto it = vp_objects.begin(); it != vp_objects.end();it++) {
		switch (_side) {
		case rect_side::top:
			if (((*it)->to.y <= -(*it)->to.h)) {
				(*it)->health_power = -1;
			}
			break;
		case rect_side::left:
			break;
		case rect_side::bottom:
			if ((*it)->to.y >= g_window_rect.h) {
				(*it)->health_power = -1;
				if (is_reduce_players_HP)	gp_player->health_power -= 5;
			}
			break;
		case rect_side::right:
			break;
		default:
			break;
		}
	}
}


void check_collisions() {

	SDL_Rect result;

	// Return vector by reference

	std::vector<std::shared_ptr<Body>>& vp_enemies = enemy_manager.get_vp_bodies();
	std::vector<std::shared_ptr<Body>>& vp_bullets = bullet_manager.get_vp_bodies();
	std::vector<std::shared_ptr<Body>>& vp_enemy_bullets = enemy_bullets.get_vp_bodies();
	display_status();

	// if the bullet collide to window top
	for (auto bullet = vp_bullets.begin(); bullet != vp_bullets.end(); /*bullet++*/) {
		if (((*bullet)->to.y <= -(*bullet)->to.h)) {
			bullet = vp_bullets.erase(bullet);
			continue;
		}
		++bullet;
	}
	// if the bullet collide to window bottom
	for (auto bullet = vp_enemy_bullets.begin(); bullet != vp_enemy_bullets.end(); ) {
		if (((*bullet)->to.y >= g_window_rect.h)) {
			bullet = vp_enemy_bullets.erase(bullet);
			continue;
		}
		++bullet;
	}

	// if the bullet collide with enemy
	for (auto enemy = vp_enemies.begin(); enemy != vp_enemies.end(); /*enemy++*/) {
		// if the obstacle collide with floor (bottom of the screen)
		if ((*enemy)->to.y >= g_window_rect.h) {
			(*enemy)->health_power = -1;
			gp_player->health_power -= 5;
		}
		// check the collision between enemy and player
		if (SDL_IntersectRect(&(gp_player->to), &((*enemy)->to), &result)) {
			(*enemy)->health_power -= (gp_player)->attack_power;
			gp_player->health_power -= (*enemy)->attack_power;
		}
		for (auto bullet = vp_bullets.begin(); bullet != vp_bullets.end(); /*bullet++*/) {
			if (SDL_IntersectRect(&((*bullet)->to), &((*enemy)->to), &result)) {
				(*enemy)->health_power -= (*bullet)->attack_power;
				(*bullet)->health_power = -1;
				bullet = vp_bullets.erase(bullet);
				gp_player->health_power += 2;
				continue;
			}
			++bullet;
		}
		if ((*enemy)->health_power <= 0) {
			enemy = vp_enemies.erase(enemy);
			g_kill_count++;
			continue;
		}
		++enemy;
	}
	// check for got enemys shoot
	for (auto bullet = vp_enemy_bullets.begin(); bullet != vp_enemy_bullets.end(); /*bullet++*/) {
		if (SDL_IntersectRect(&((*bullet)->to), &(gp_player->to), &result)) {
			gp_player->health_power -= (*bullet)->attack_power;
			bullet = vp_enemy_bullets.erase(bullet);
			continue;
		}
		++bullet;
	}
}

int main(int argc, char* args[])
{
	if (!game_init()) return -1;

	const Uint32 FPS = 60;
	const Uint32 frame_delay = 1000 / FPS;
	Uint32	frame_start = 0;
	Uint32 frame_interval = 100;

	while (true) {
		frame_start = SDL_GetTicks();
		game_update();
		// clean blackboard before drawing
		SDL_RenderClear(gp_renderer);

		background_manager.display();
		player_manager.display();
		enemy_manager.display();
		bullet_manager.display();
		enemy_bullets.display();

		// show from now
		SDL_RenderPresent(gp_renderer);

		// adjust frame rate
		frame_interval = SDL_GetTicks() - frame_start;
		if (frame_delay > frame_interval) {
			SDL_Delay(frame_delay - frame_interval);
		}
	}
	SDL_DestroyRenderer(gp_renderer);
	SDL_DestroyWindow(gp_window);
	SDL_Quit();
	return 0;
}
