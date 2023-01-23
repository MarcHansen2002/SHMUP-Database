#include <assert.h>

#include "GameObj.h"
#include "Game.h"

using namespace sf;
using namespace std;


void GameObj::InitShip(RenderWindow& window, Texture& tex)
{
	spr.setTexture(tex, true);
	const IntRect& texRect = spr.getTextureRect();
	spr.setOrigin(texRect.width / 2.f, texRect.height / 2.f);
	spr.setScale(0.2f, 0.2f);
	spr.setRotation(90);
	type = ObjectT::Ship;
	radius = 25.f;
	ResetShip(window);
}

void GameObj::ResetShip(RenderWindow& window)
{
	health = 3;
	active = true;
	spr.setPosition(spr.getGlobalBounds().width*0.6f, window.getSize().y / 2.f);
}

void GameObj::InitRock(RenderWindow& window, Texture& tex)
{
	spr.setTexture(tex);
	IntRect texR(0, 0, 96, 96);
	spr.setTextureRect(texR);
	spr.setOrigin(texR.width / 2.f, texR.height / 2.f);
	radius = GC::ROCK_RAD.x + (float)(rand() % (int)GC::ROCK_RAD.y);
	float scale = 0.75f * (radius / 25.f);
	spr.setScale(scale, scale);
	active = false;
	type = ObjectT::Rock;
	ResetRock();
}

void GameObj::ResetRock()
{
	health = (int)(5.f * spr.getScale().x);
}

void GameObj::InitEnemy(RenderWindow& window, Texture& tex)
{
	spr.setTexture(tex);
	IntRect texR(0, 0, 50, 50);
	spr.setTextureRect(texR);
	spr.setOrigin(texR.width / 2.f, texR.height / 2.f);
	radius = 25;
	spr.setRotation(-90);

	type = ObjectT::Enemy;
	ResetEnemy();
}

void GameObj::ResetEnemy()
{
	health = 1;
}


void GameObj::InitBullet(RenderWindow& window, Texture& tex)
{
	spr.setTexture(tex);
	IntRect texR(0, 0, 32, 32);
	spr.setTextureRect(texR);
	spr.setOrigin(texR.width / 2.f, texR.height / 2.f);
	radius = 5.f;
	float scale = 0.5f;
	spr.setScale(scale, scale);
	active = false;
	type = ObjectT::Bullet;
	health = 0;
}

void GameObj::Init(RenderWindow& window, Texture& tex, ObjectT type_, Game& game)
{
	pGame = &game;
	switch (type_)
	{
	case ObjectT::Ship:
		InitShip(window, tex);
		break;
	case ObjectT::Rock:
		InitRock(window, tex);
		break;
	case ObjectT::Bullet:
		InitBullet(window, tex);
		break;
	case ObjectT::Enemy:
		InitEnemy(window, tex);
		break;
	default:
		assert(false);
	}
}

void GameObj::Update(RenderWindow& window, float elapsed, bool fire)
{
	if (active)
	{
		colliding = false;
		switch (type)
		{
		case ObjectT::Ship:
			PlayerControl(window.getSize(), elapsed, fire);
			break;
		case ObjectT::Rock:
			MoveRock(elapsed);
			break;
		case ObjectT::Bullet:
			MoveBullet(window.getSize(), elapsed);
			break;
		case ObjectT::Enemy:
			EnemyShoot(elapsed);
			MoveEnemy(window.getSize(), elapsed);
			break;
		}
	}
}

void GameObj::MoveRock(float elapsed)
{
	const Vector2f& pos = spr.getPosition();
	float x = pos.x - GC::ROCK_SPEED * elapsed;
	if (x < -spr.getGlobalBounds().width / 2.f)
		active = false;
	spr.setPosition(x, pos.y);
}

void GameObj::EnemyShoot(float elapsed)
{
}


void GameObj::MoveEnemy(const sf::Vector2u& screenSz, float elapsed)
{
	//where is the player?
	assert(pGame);
	bool evade = false;
	sf::Vector2f inc;
	//for (size_t i = 0; i < pGame->objects.size(); ++i)
	//{
	//	if (pGame->objects[i].active)
	//	{
	//		GameObj& o = pGame->objects[i];
	//		if ((o.type == ObjectT::Enemy) && (&o != this))
	//		{
	//			const float MinDist = 0;
	//			sf::Vector2f& direction = o.spr.getPosition() - spr.getPosition();
	//			float dist = direction.x * direction.x + direction.y * direction.y;
	//			
	//			if (dist <= MinDist)
	//			{
	//				evade = true;
	//				inc = -direction * elapsed * GC::ENEMY_SPEED;
	//			}
	//		}
	//	}
	//}
	//
	////Go towards player
	//if (!evade)
	//{
	//	GameObj& ship = pGame->objects[0];
	//	sf::Vector2f& dir = ship.spr.getPosition() - spr.getPosition();
	//	float d = dir.x * dir.x + dir.y * dir.y;
	//	d = sqrtf(d);
	//	dir /= d;
	//	inc = dir * elapsed * GC::ENEMY_SPEED;
	//}
	
	sf::Vector2f pos = spr.getPosition();
	if (pos.x <= -spr.getGlobalBounds().width / 2)
	{
		spr.setPosition(pos.x += GC::SCREEN_RES.x, pos.y);
	}
	pos.x += -1 * GC::ENEMY_SPEED * elapsed;
	//pos += inc;
	spr.setPosition(pos);
}


void GameObj::MoveBullet(const sf::Vector2u& screenSz, float elapsed)
{
	const Vector2f& pos = spr.getPosition();
	float x;
	if (pMySpawner && pMySpawner->type == ObjectT::Ship)
	{
		x = pos.x + 250 * elapsed;
		if (x > (screenSz.x + spr.getGlobalBounds().width / 2.f))
			active = false;
	}
	else
	{
		x = pos.x - GC::ENEMY_BULLET_SPEED * elapsed;
		if (x < 0)
			active = false;
	}
	spr.setPosition(x, pos.y);
}

void GameObj::Render(RenderWindow& window, float elapsed)
{
	if (active)
		window.draw(spr);
}

/*
reduce a vector by a certain percentage over a certain time
currentVel - the vector you need to make a reduced version of
pcnt - at the end of timeInterval it should have reduced by this percentage (e.g. 10%=0.1)
timeInterval - how many seconds for the pcnt reduction to complete
dTimeS - actual elapsed time e.g. 10% after 1sec, so if dTimeS=0.1s, then the reduction will be 1%
*/
Vector2f Decay(Vector2f& currentVal, float pcnt, float timeInterval, float dTimeS)
{
	float mod = 1.0f - pcnt * (dTimeS / timeInterval);
	Vector2f alpha(currentVal.x * mod, currentVal.y * mod);
	return alpha;
}

void GameObj::PlayerControl(const Vector2u& screenSz, float elapsed, bool fire)
{
	Vector2f pos = spr.getPosition();
	const float SPEED = 250.f;
	FloatRect rect = spr.getGlobalBounds();

	static Vector2f thrust{ 0,0 };

	if (Keyboard::isKeyPressed(Keyboard::Up) ||
		Keyboard::isKeyPressed(Keyboard::Down) ||
		Keyboard::isKeyPressed(Keyboard::Left) ||
		Keyboard::isKeyPressed(Keyboard::Right))
	{
		if (Keyboard::isKeyPressed(Keyboard::Up))
			thrust.y = -SPEED;
		else if (Keyboard::isKeyPressed(Keyboard::Down))
			thrust.y = SPEED;
		if (Keyboard::isKeyPressed(Keyboard::Left))
			thrust.x = -SPEED;
		else if (Keyboard::isKeyPressed(Keyboard::Right))
			thrust.x = SPEED;
	}

	pos += thrust * elapsed;
	thrust = Decay(thrust, 0.1f, 0.02f, elapsed);

	if (pos.y < (rect.height*0.6f))
		pos.y = rect.height*0.6f;
	if (pos.y > (screenSz.y - rect.height*0.6f))
		pos.y = screenSz.y - rect.height*0.6f;
	if (pos.x < (rect.width*0.6f))
		pos.x = rect.width*0.6f;
	if (pos.x > (screenSz.x - rect.width*0.6f))
		pos.x = screenSz.x - rect.width*0.6f;

	spr.setPosition(pos);

	if (fire)
	{
		Vector2f pos(pos.x + spr.getGlobalBounds().width / 2.f, pos.y);
		FireBullet(pos);
	}
}

void GameObj::FireBullet(const Vector2f& pos)
{
	assert(pGame);
	size_t idx = 0;
	bool found = false;
	while (idx < pGame->objects.size() && !found)
	{
		if (!pGame->objects[idx].active && pGame->objects[idx].type == ObjectT::Bullet)
			found = true;
		else
			++idx;
	}
	if (idx < pGame->objects.size())
	{
		GameObj& bullet = pGame->objects[idx];
		bullet.active = true;
		bullet.spr.setPosition(pos);
		if (type == ObjectT::Ship)
			bullet.spr.setColor(Color(128, 128, 255, 255));
		else
			bullet.spr.setColor(Color(255, 128, 128, 255));
		bullet.pMySpawner = this;
	}
}

void EnemySplash(Game& game, const Vector2f& pos)
{
	Emitter* em = game.particleSys.GetNewEmitter();
	if (em)
	{
		em->numToEmit = 500;
		em->numAtOnce = 10;
		em->pos = pos;
		em->rate = 0.001f;
		em->colour = Color(90, 20, 20, 255);
		em->scale = Vector2f{ 0.25f,0.25f };
		em->life = 0.75f;
		em->initSpeed = Dim2Di{ 50,75 };
		em->initVel = Vector2f(0, 0);
	}
}


void BulletSplash(Game& game, const Vector2f& pos, const Vector2f& initVel)
{
	Emitter* em = game.particleSys.GetNewEmitter();
	if (em)
	{
		em->numToEmit = 30;
		em->numAtOnce = 30;
		em->pos = pos;
		em->rate = 0.001f;
		em->colour = Color(128, 50, 50, 255);
		em->scale = Vector2f{ 0.15f,0.15f };
		em->life = 0.25f;
		em->initSpeed = Dim2Di{ 15,20 };
		em->initVel = initVel;
	}
}

void RockExplode(Game& game, const Vector2f& pos, float radius)
{
	Emitter* em = game.particleSys.GetNewEmitter();
	if (em)
	{
		em->numToEmit = 500;
		em->numAtOnce = 20;
		em->pos = pos;
		em->rate = 0.001f;
		em->colour = Color(10, 10, 19, 255);
		float sizeMult = (radius / GC::ROCK_RAD.y) * 2;
		em->scale = Vector2f{ 0.5f,0.5f };
		em->life = 1.f;
		em->initSpeed.y = 25 + (int)(50 * sizeMult);
		em->initSpeed.x = (int)(em->initSpeed.y * 0.5f);
		em->initVel = Vector2f{ 0,0 };
	}
}

void EnemyExplode(Game& game, const Vector2f& pos, float radius)
{
	Emitter* em = game.particleSys.GetNewEmitter();
	if (em)
	{
		em->numToEmit = 500;
		em->numAtOnce = 20;
		em->pos = pos;
		em->rate = 0.001f;
		em->colour = Color(10, 10, 19, 255);
		float sizeMult = (radius / GC::ROCK_RAD.y) * 2;
		em->scale = Vector2f{ 0.5f,0.5f };
		em->life = 1.f;
		em->initSpeed.y = 25 + (int)(50 * sizeMult);
		em->initSpeed.x = (int)(em->initSpeed.y * 0.5f);
		em->initVel = Vector2f{ 0,0 };
	}
}

void ShipExplode(Game& game, const Vector2f& pos, const Vector2f& initVel)
{
	Emitter* em = game.particleSys.GetNewEmitter();
	if (em)
	{
		em->numToEmit = 1000;
		em->numAtOnce = 50;
		em->pos = pos;
		em->rate = 0.001f;
		em->colour = Color(10, 20, 20, 255);
		em->scale = Vector2f{ 0.5f,0.75f };
		em->life = 1.f;
		em->initSpeed = Dim2Di{ 50,350 };
		em->initVel = initVel;
	}
}

void GameObj::Hit(GameObj& other)
{
	switch (type)
	{
	case ObjectT::Ship:
		{
			bool spawnedByMe = other.pMySpawner && other.pMySpawner->type == type;
			if (!spawnedByMe)
				other.TakeDamage(999, *this);
			break;
		}
	case ObjectT::Bullet:
		{
			bool spawnedByOther = pMySpawner->type == other.type;
			if (!spawnedByOther && other.type!=ObjectT::Bullet)
				other.TakeDamage(1, *this);
			break;
		}
	case ObjectT::Rock:
		other.TakeDamage(1, *this);
		break;
	case ObjectT::Enemy:
		{
			//ignore things I spawned
			//bool spawnedByMe = other.pMySpawner && other.pMySpawner->type == type;
			//if (!spawnedByMe)
			other.TakeDamage(1, *this);
			break;
		}
	default:
		assert(false);
	}
}

void GameObj::TakeDamage(int amount, GameObj& other)
{
	assert(pGame);

	health -= amount;
	if (health <= 0)
 		active = false;

	switch (type)
	{
	case ObjectT::Ship:
		ShipExplode(*pGame, spr.getPosition(), Vector2f{ 0,0 });
		assert(pGame);
		pGame->metrics.lives--;
		break;
	case ObjectT::Bullet:
  		BulletSplash(*pGame, spr.getPosition(), Vector2f{ -GC::ROCK_SPEED,0 });
		break;
	case ObjectT::Rock:
		if (health <= 0)
			RockExplode(*pGame, other.spr.getPosition(), radius);
	case ObjectT::Enemy:
		if (health <= 0)
			EnemyExplode(*pGame, other.spr.getPosition(), radius);
			pGame->metrics.score += 1;
		break;
	default:
		assert(false);
	}
}
