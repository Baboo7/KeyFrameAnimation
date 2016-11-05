#include "time_line.hpp"
#include <iostream>
#include <math.h>

using namespace std ;

TimeLine::TimeLine (sf::Vector2f up_left, sf::Vector2f bot_right)
{
    rect.setSize(bot_right-up_left) ;
    rect.setPosition(up_left) ;
    rect.setFillColor(sf::Color(64,224,208)) ;

    nb_max_keyframes = 200 ;
    spacing_keyframes = (float) rect.getSize().x / nb_max_keyframes ;
    spacing_keyframes += spacing_keyframes/(float)nb_max_keyframes ;
    animation_duration = 1000.0f ;
    time_between_keyframes = (float)animation_duration / nb_max_keyframes ;
    time_between_keyframes += time_between_keyframes/(float)nb_max_keyframes ;

    offset = 0.0f ;

    cursor = 0 ;
    rect_cursor.setSize(sf::Vector2f(2.0f, bot_right.y-up_left.y)) ;
    rect_cursor.setPosition(rect.getPosition() + sf::Vector2f((float) cursor * spacing_keyframes, 0.0f)) ;
    rect_cursor.setFillColor(sf::Color::Red) ;

    origine = sf::Vector2f(0,0) ;
    circle_origine.setRadius (3.0f) ;
    circle_origine.setOrigin(sf::Vector2f(1.5f,1.5f)) ;
    circle_origine.setPosition(origine) ;
    circle_origine.setFillColor(sf::Color(100,170,200,160)) ;

    bPlayAnimation = false ;
    bUpdateSpritesPositionOnCursorMove = false ;
}

bool TimeLine::IsClicked(const sf::Vector2f mouse, TreeItem* root)
{
    if (rect.getGlobalBounds().contains(mouse))
    {
        // On positionne le curseur à l'endroit du clic.
        cursor = iquo((float) mouse.x-rect.getPosition().x, spacing_keyframes) ;
        rect_cursor.setPosition(rect.getPosition() + sf::Vector2f((float) (cursor * spacing_keyframes), 0.0f)) ;

        // Mise à jour de la position des sprites.
        if (bUpdateSpritesPositionOnCursorMove)
        {
            KeyFrame key_1 ;
            KeyFrame key_2 ;
            float time =0 ;
            if (cursor == 0) time = (float)(nb_max_keyframes)*time_between_keyframes ;
            else time = (float)(cursor)*time_between_keyframes ;

            GetKeyframes(time, &key_1, &key_2) ;

            key_1 = InterpolatePosition(time, &key_1, &key_2) ;

            UpdateSpritesPositionsRecursive(root, &key_1) ;
        }

        return true ;
    }

    return false ;
}

void TimeLine::MoveCursor (direction_e d, TreeItem* root)
{
    // Déplacement du curseur.
    switch (d)
    {
    case LEFT:
        if (cursor == 0)
            cursor = nb_max_keyframes-1 ;
        else
            cursor-- ;

        rect_cursor.setPosition(rect.getPosition() + sf::Vector2f((float) (cursor * spacing_keyframes), 0.0f)) ;
        break ;
    case RIGHT:
        if (cursor == nb_max_keyframes-1)
            cursor = 0 ;
        else
            cursor++ ;

        rect_cursor.setPosition(rect.getPosition() + sf::Vector2f((float) (cursor * spacing_keyframes), 0.0f)) ;
        break ;
    }

    // Mise à jour de la position des sprites.
    if (bUpdateSpritesPositionOnCursorMove)
    {
        KeyFrame key_1 ;
        KeyFrame key_2 ;
        float time =0 ;
        if (cursor == 0) time = (float)(nb_max_keyframes)*time_between_keyframes ;
        else time = (float)(cursor)*time_between_keyframes ;
        GetKeyframes(time, &key_1, &key_2) ;

        KeyFrame key = InterpolatePosition(time, &key_1, &key_2) ;

        UpdateSpritesPositionsRecursive(root, &key) ;
    }
}

void TimeLine::SetPlayAnimation(bool bPlay)
{
    bPlayAnimation = bPlay ;

    if (bPlayAnimation)
    {
        timer.restart() ;
    }
    else
    {
        offset = (float) cursor * time_between_keyframes ;
    }
}

void TimeLine::StopAnimation(TreeItem *root)
{
    SetPlayAnimation(false) ;
    offset = 0.0f ;
    UpdateCursor(offset) ;
    UpdateSpritesPositionsRecursive(root, &table_keyframes[0]) ;
}

void TimeLine::Draw(sf::RenderWindow &window)
{
    window.draw(rect) ;

    for (unsigned int i=0 ; i<table_keyframes.size() ; i++)
        window.draw(table_keyframes[i].rect) ;

    window.draw(rect_cursor) ;
}

void TimeLine::AddKeyframe (vector<sprite_t *> &table_sprites)
{
    KeyFrame* k = GetKeyframe (cursor) ;

    if (k == NULL)
        InitialiseKeyFrame(cursor, table_sprites) ;
    else
        UpdateKeyframe (k, table_sprites) ;
}

void TimeLine::DeleteKeyframe()
{
    // On ne peut pas supprimer la première key frame.
    if (cursor == 0) return ;

    KeyFrame* k = GetKeyframe (cursor) ;
    if (k != NULL)
    {
        unsigned int i = 0;
        while (i<table_keyframes.size())
        {
            if (table_keyframes[i].time == k->time)
                table_keyframes.erase(table_keyframes.begin() + i) ;

            i++ ;
        }
    }
}

void TimeLine::AnimationHandler(TreeItem *root)
{
    if (!bPlayAnimation) return ;

    float time_animation = irem((float)timer.getElapsedTime().asMilliseconds() + offset, animation_duration) ;

    KeyFrame key_1 ;
    KeyFrame key_2 ;
    GetKeyframes(time_animation, &key_1, &key_2) ;

    KeyFrame key = InterpolatePosition(time_animation, &key_1, &key_2) ;

    UpdateSpritesPositionsRecursive(root, &key) ;

    UpdateCursor (time_animation) ;
}

void TimeLine::UpdateCursor (float time)
{
    cursor = iquo(time, time_between_keyframes) ;
    rect_cursor.setPosition(rect.getPosition() + sf::Vector2f((float) (cursor * spacing_keyframes), 0.0f)) ;
}

KeyFrame* TimeLine::GetKeyframe (unsigned int k)
{
    unsigned int i = 0 ;
    KeyFrame* key = NULL ;
    while (i < table_keyframes.size())
    {
        if (table_keyframes[i].time > k) // Si on dépasse la valeur recherhchée, on arrête l'algorithme (tableau trié dans l'ordre croissant).
            break ;
        else if (table_keyframes[i].time == k)
        {
            key = &table_keyframes[i] ;
            break ;
        }

        i++ ;
    }

    return key ;
}

void TimeLine::GetKeyframes (float time, KeyFrame *key_1, KeyFrame *key_2)
{
    unsigned int index = 0 ;
    while (index<table_keyframes.size())
    {
        if ((float) (table_keyframes[index].time*time_between_keyframes) >= time) break ;
        index++ ;
    }

    index %= table_keyframes.size() ;
    *key_2 = table_keyframes[index] ;
    if (index == 0) index = table_keyframes.size()-1 ;
    else index -- ;
    *key_1 = table_keyframes[index] ;
}

KeyFrame TimeLine::InterpolatePosition (float time, KeyFrame *key_1, KeyFrame *key_2)
{
    float percentage = 0.0f ;
    if (key_2->time - key_1->time != 0)
    {
        if (key_2->time == 0)
            percentage = (static_cast<float>(time)/time_between_keyframes - key_1->time) / abs((int)(nb_max_keyframes - key_1->time)) ; // nb_max_keyframes = animation_duration/time_between_keyframes
        else
            percentage = (static_cast<float>(time)/time_between_keyframes - key_1->time) / abs((int)(key_2->time - key_1->time)) ;

        if (percentage > 1.0f) percentage = 1.0f ;
    }

    // On interpole les positions de chaque sprite et on sauvegarde le résultat dans key.
    KeyFrame key ;
    key.table_positions.resize(key_1->table_positions.size()) ;
    for (unsigned int i=0; i<key_1->table_positions.size() ; i++)
    {
        key.table_positions[i].position = key_1->table_positions[i].position*(1.0f - percentage) + key_2->table_positions[i].position*percentage ;
        key.table_positions[i].rotation = key_1->table_positions[i].rotation*(1.0f - percentage) + key_2->table_positions[i].rotation*percentage ;
    }

    return key ;
}

void TimeLine::UpdateSpritesPositionsRecursive (TreeItem* item, KeyFrame *key)
{
    item->m_sprite.sprite.setPosition(key->table_positions[item->m_sprite.ID].position) ;
    item->m_sprite.sprite.setRotation(key->table_positions[item->m_sprite.ID].rotation) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++)
        UpdateSpritesPositionsRecursive(item->children[i], key) ;
}

void TimeLine::UpdateKeyframe (KeyFrame* key, vector<sprite_t *> &table_sprites)
{
    for (unsigned int i=0 ; i<key->table_positions.size() ; i++)
    {
        key->table_positions[table_sprites[i]->ID].position = table_sprites[i]->sprite.getPosition() ;
        key->table_positions[table_sprites[i]->ID].rotation = table_sprites[i]->sprite.getRotation() ;
    }
}

void TimeLine::SetKeyframePosition (KeyFrame &key)
{
    key.rect.setPosition(rect.getPosition() + sf::Vector2f((float) (key.time * spacing_keyframes), 0.0f)) ;
}

/*
 *  time doit être compris entre 0 et nb_max_keyframes.
 */
void TimeLine::InitialiseKeyFrame(unsigned int time, vector<sprite_t *> &table_sprites)
{
    // Création de la keyframe
    KeyFrame key ;
        key.time = time ;

        key.rect.setSize(sf::Vector2f(2.0f, rect.getSize().y)) ;
        SetKeyframePosition(key) ;
        key.rect.setFillColor(sf::Color::Yellow) ;

        key.table_positions.resize(table_sprites.size()) ;
        UpdateKeyframe (&key, table_sprites) ; // On sauvegarde la position/rotation des sprites dans le tableau de position_t.

    // On range le tableau de keyframes dans l'ordre croissant des temps.
    unsigned int i = 0 ;
    while (i<table_keyframes.size())
    {
        if (table_keyframes[i].time > key.time)
        {
            table_keyframes.insert(table_keyframes.begin() + i, key) ;
            return ;
        }

        i++ ;
    }

    table_keyframes.push_back(key) ; // Ajout de la keyframe au tableau de keyframe.
}

void TimeLine::ReinitTimeLine (float &new_anim_duration, unsigned int &new_nb_max_key, float &new_time_between_key, sf::Vector2f &new_origine, vector<KeyFrame> &table_keys, TreeItem *root)
{
    nb_max_keyframes = new_nb_max_key ;
    time_between_keyframes = new_time_between_key ;
    animation_duration = new_anim_duration ;
    spacing_keyframes = static_cast<float> (rect.getSize().x) / nb_max_keyframes ;

    UpdateCursor(0.0f) ;
    offset = 0.0f ;

    SetAnimationOrigine(new_origine) ;
    table_keyframes = table_keys ;
    for (unsigned int i = 0 ; i < table_keyframes.size() ; i ++)
    {
        table_keyframes[i].rect.setSize(sf::Vector2f(2.0f, rect.getSize().y)) ;
        SetKeyframePosition(table_keyframes[i]) ;
        table_keyframes[i].rect.setFillColor(sf::Color::Yellow) ;
    }

    UpdateSpritesPositionsRecursive (root, &table_keyframes[0]) ;
}

unsigned int TimeLine::iquo (float dividend, float divisor)
{
    if (divisor == 0) return dividend ;

    unsigned int quotient = 0 ;
    while (dividend > divisor)
    {
        dividend -= divisor ;
        quotient++ ;
    }

    return quotient ;
}

float TimeLine::irem (float dividend, float divisor)
{
    if (divisor == 0) return dividend ;

    while (dividend > divisor) dividend -= divisor ;

    return dividend ;
}

void TimeLine::SetAnimationDuration(float milliseconds)
{
    animation_duration = milliseconds ;
    time_between_keyframes = static_cast<float> (animation_duration) / nb_max_keyframes ;
}

