#include "sprite_manager.hpp"
#include <iostream>
using namespace std ;

void SwapSprites(int i1, int i2, vector<sprite_t *> &table) ;

void InitialiseSpritesPosition(vector<sprite_t *> *sprites_displaying, int X_o, int Y_o, int width, int height)
{
    int X = X_o ;
    int Y = Y_o ;
    int max_sprite_height = 0 ;

    for(unsigned int i=0 ; i<sprites_displaying->size() ; i++)
    {
        (*sprites_displaying)[i]->sprite.setPosition(sf::Vector2f(X,Y)) ;
        if ((*sprites_displaying)[i]->sprite.getLocalBounds().height > max_sprite_height)
            max_sprite_height = (*sprites_displaying)[i]->sprite.getLocalBounds().height ;

        X += (*sprites_displaying)[i]->sprite.getLocalBounds().width ;

        if (X >= width)
        {
            X = X_o ;
            Y += max_sprite_height ;
            max_sprite_height = 0 ;

            if (Y >= height)
                Y = Y_o ;
        }
    }
}

void DrawAnimationSprites(sf::RenderWindow *window, vector<sprite_t *> *sprites_displaying)
{
    for(unsigned int i=0 ; i<sprites_displaying->size() ; i++) // Le sprite ayant l'index le plus faible est affiché à l'avant.
        window->draw((*sprites_displaying)[(*sprites_displaying).size()-i-1]->sprite) ;
}

void SelectSpriteFromTree(unsigned int ID, std::vector<sprite_t *> *sprites_displaying)
{
    for (unsigned int i=0 ; i<(*sprites_displaying).size() ; i++)
    {
        if ((*sprites_displaying)[i]->ID == ID && (*sprites_displaying)[i]->bSelected == false)
            SelectSprite((*sprites_displaying)[i]) ;
        else if ((*sprites_displaying)[i]->ID != ID && (*sprites_displaying)[i]->bSelected == true)
            UnselectSprite((*sprites_displaying)[i]) ;
    }
}

/*
    Sélectionne un sprite en particulier.
 */
void SelectSprite(sprite_t *spr)
{
    spr->sprite.setColor(sf::Color(0,0,255,255)) ;
    spr->bSelected = true ;

    // Affichage du centre de gravité du sprite.
}

/*
    Désélectionne tous les sprites.
 */
void UnselectSprites(std::vector<sprite_t *> *sprites_displaying)
{
    for (unsigned int i=0 ; i<(*sprites_displaying).size() ; i++)
    {
        if ((*sprites_displaying)[i]->bSelected)
            UnselectSprite((*sprites_displaying)[i]) ;
    }
}

/*
    Désélectionne un sprite en particulier.
 */
void UnselectSprite(sprite_t *spr)
{
    spr->sprite.setColor(sf::Color(255,255,255,180)) ;
    spr->bSelected = false ;

    // Arrêter l'affichage du centre de gravité.
}

int GetSpriteTableIndex(sprite_t* spr, std::vector<sprite_t *> &table)
{
    unsigned int i = 0 ;
    while (i < table.size())
    {
        if (table[i]->ID == spr->ID)
            return i ;

        i++ ;
    }

    // Le sprite n'a pas été retrouvé, on retourne -1.
    return -1 ;
}

void SpritesClicHandler(std::vector<sprite_t *> *sprites_displaying, const sf::Vector2f &mouse, TreeItem *root)
{
    sprite_t *spr = NULL ;
    for (unsigned int i = 0 ; i < (*sprites_displaying).size() ; i++)
    {
        if ((*sprites_displaying)[(*sprites_displaying).size()-i-1]->sprite.getGlobalBounds().contains(mouse))
            spr = (*sprites_displaying)[(*sprites_displaying).size()-i-1] ;
        else if ((*sprites_displaying)[(*sprites_displaying).size()-i-1]->bSelected == true)
            UnselectSprite((*sprites_displaying)[(*sprites_displaying).size()-i-1]) ;
    }

    if (spr != NULL)
    {
        SelectSprite(spr) ;

        // Mise à jour de l'arbre.
        UnselectPushButtons(root) ;
        SelectPushButtonFromAnimationWindow(root, spr->ID) ;
    }
    else // Si aucun bouton n'a été cliqué, on désélectionne tous les PushButtons.
        UnselectPushButtons(root) ;
}

sprite_t* GetSelectedSprite(std::vector<sprite_t *> *sprites_displaying)
{
    for (unsigned int i=0 ; i<sprites_displaying->size() ; i++)
    {
        if ((*sprites_displaying)[sprites_displaying->size()-i-1]->bSelected)
            return (*sprites_displaying)[sprites_displaying->size()-i-1] ;
    }

    return NULL ;
}

void MoveSpriteUp (sprite_t* spr, std::vector<sprite_t *> &table)
{
    int index = GetSpriteTableIndex(spr, table) ;

    if (index > 0)
        SwapSprites(index, index-1, table) ;
}

void MoveSpriteDown (sprite_t* spr, std::vector<sprite_t *> &table)
{
    int index = GetSpriteTableIndex(spr, table) ;

    if (index == -1)
        return ;

    if ((unsigned int) index < table.size()-1)
        SwapSprites(index, index+1, table) ;
}

void SwapSprites(int i1, int i2, vector<sprite_t *> &table)
{
    sprite_t *tmp = table[i1] ;
    table[i1] = table[i2] ;
    table[i2] = tmp ;

}

void SetNewOrigin (sprite_t* spr, sf::Vector2f new_origine)
{
    spr->sprite.setOrigin(spr->sprite.getOrigin() + new_origine - spr->sprite.getPosition()) ;
    spr->sprite.setPosition(new_origine) ; // Corrige le bug de saut au changement de centre de gravité.
}











