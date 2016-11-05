#ifndef _H_TIMELINE_
#define _H_TIMELINE_

#include <SFML/Graphics.hpp>
#include <vector>
#include "tree.hpp"

enum direction_e
{
    LEFT,
    RIGHT
};

struct position_t
{
    sf::Vector2f position ; // Position du sprite dans la fen�tre.
    float rotation ; // Rotation du sprite par rapport � l'axe x.
};

struct KeyFrame
{
    unsigned int time ; // Position de la key frame sur la time line en pourcentage (0 - nb_max_keyframes).
    std::vector<position_t> table_positions ; // Contient la position de chaque sprite � la date time.
    sf::RectangleShape rect ; // Affichage sur la time line.
};



/*
 *  La classe TimeLine g�re les images cl�s et la lecture de l'animation. Elle peut enregistrer jusqu'�  images cl�s.
 *  La dur�e de l'animation peut �tre modul�e en jouant sur sa vitesse de lecture.
 */
class TimeLine
{
public:
    TimeLine (sf::Vector2f up_left, sf::Vector2f bot_right) ;

    bool IsClicked (const sf::Vector2f mouse, TreeItem* root) ;
    void MoveCursor (direction_e d, TreeItem* root) ;
    void SetPlayAnimation (bool bPlay) ;
    void StopAnimation (TreeItem *root) ;
    void Draw(sf::RenderWindow &window) ;
    void AddKeyframe (std::vector<sprite_t *> &table_sprites) ;
    void DeleteKeyframe () ;
    void AnimationHandler (TreeItem *root) ;

    static unsigned int iquo (float dividend, float divisor) ;
    static float irem (float dividend, float divisor) ;

    void SetAnimationDuration (float milliseconds) ;
    void SetAnimationOrigine (const sf::Vector2f &o) {origine = o ; circle_origine.setPosition(origine) ; } ;
    sf::CircleShape circle_origine ;

    unsigned int GetNbMaxKeyframes () const { return nb_max_keyframes ; } ;
    float GetAnimationDuration () const { return animation_duration ; } ;
    float GetTimeBetweenKeyframes () const { return time_between_keyframes ; } ;
    std::vector<KeyFrame>* GetTableKeyframes () { return &table_keyframes ; } ;
    sf::Vector2f GetAnimationOrigine () const { return origine ; } ;
    // Permet de demander l'actualisation de la position des sprites ou non lorsque la position du curseur est modifi�e.
    void SetUpdateSpritesPositionOnCursorMove (bool b) { bUpdateSpritesPositionOnCursorMove = b ; } ;
    void ToggleUpdateSpritesPositionOnCursorMove () { bUpdateSpritesPositionOnCursorMove = !bUpdateSpritesPositionOnCursorMove ; } ;

    /* Utilis�e apr�s avoir charg� un nouveu sprite et une nouvelle animation. */
    void ReinitTimeLine (float &new_anim_duration, unsigned int &new_nb_max_key, float &new_time_between_key, sf::Vector2f &new_origine, std::vector<KeyFrame> &table_keys, TreeItem *root) ;

private:
    void SetKeyframePosition (KeyFrame &key) ;
    void InitialiseKeyFrame (unsigned int time, std::vector<sprite_t *> &table_sprites) ;
    void UpdateKeyframe (KeyFrame* k, std::vector<sprite_t *> &table_sprites) ;
    KeyFrame* GetKeyframe (unsigned int k) ;
    void GetKeyframes (float time, KeyFrame *key_1, KeyFrame *key_2) ;
    KeyFrame InterpolatePosition (float time, KeyFrame *key_1, KeyFrame *key_2) ;
    void UpdateSpritesPositionsRecursive (TreeItem* item, KeyFrame *key) ;
    void UpdateCursor (float time) ;

protected:
    sf::RectangleShape rect ; // Affichage de la time line.
    sf::RectangleShape rect_cursor ; // Affichage du curseur.
    unsigned int cursor ; // Position du curseur sur la time line en pourcentage.
    std::vector<KeyFrame> table_keyframes ; // Tableau de key frames rang� dans l'odre croissant de leur attribut time.
    unsigned int nb_max_keyframes ; // Nombre maximal que peut contenir la time line.
    float spacing_keyframes ; // Espace entre les keyframes sur la time line;
    float time_between_keyframes ; // Temps s�parant deux key frames (en ms).
    float animation_duration ; // Dur�e totale de l'animation (en ms).
    float offset ;

    sf::Clock timer ;

    bool bPlayAnimation ;
    bool bUpdateSpritesPositionOnCursorMove ; // Indique si la position des sprites doit �tre actualis�e ou non lorsque la position du curseur est modifi�e.
    sf::Vector2f origine ; // Origine du plan dans lequel est jou�e l'animation.
};

#endif // _H_TIMELINE_
