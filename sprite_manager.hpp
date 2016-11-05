#ifndef _SPRITE_MANAGER_H_
#define _SPRITE_MANAGER_H_

#include "tree.hpp"


void InitialiseSpritesPositionOnAnimationRect (std::vector<sprite_t *> *sprites_displaying, int X_o, int Y_o, int width, int height) ;

void DrawAnimationSprites(sf::RenderWindow *window, std::vector<sprite_t *> *sprites_displaying) ;

void SelectSpriteFromTree(unsigned int ID, std::vector<sprite_t *> *sprites_displaying) ;
void SelectSprite(sprite_t *spr) ;
void UnselectSprites(std::vector<sprite_t *> *sprites_displaying) ; // Utilisée par SelectSpriteFromTree.
void UnselectSprite(sprite_t *spr) ;
int GetSpriteTableIndex(sprite_t* spr, std::vector<sprite_t *> &table) ;

void SpritesClicHandler(std::vector<sprite_t *> *sprites_displaying, const sf::Vector2f &mouse, TreeItem *root) ;
sprite_t* GetSelectedSprite(std::vector<sprite_t *> *sprites_displaying) ;
void MoveSpriteUp (sprite_t* spr, std::vector<sprite_t *> &table) ;
void MoveSpriteDown (sprite_t* spr, std::vector<sprite_t *> &table) ;

void SetNewOrigin (sprite_t* spr, sf::Vector2f new_origine) ;

#endif // _SPRITE_MANAGER_H_
