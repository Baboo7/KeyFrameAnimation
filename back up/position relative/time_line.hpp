#ifndef _H_TIMELINE_
#define _H_TIMELINE_

#include <Graphics.hpp>
#include <vector>
#include "tree.hpp"

enum direction_e
{
    LEFT,
    RIGHT
};

struct position_t
{
    sf::Vector2f position ; // Position du sprite dans la fenêtre.
    float rotation ; // Rotation du sprite par rapport à l'axe x.
};

struct KeyFrame
{
    unsigned int time ; // Position de la key frame sur la time line en pourcentage (0 - nb_max_keyframes).
    std::vector<position_t> table_positions ; // Contient la position de chaque sprite à la date time.
    sf::RectangleShape rect ; // Affichage sur la time line.
};



/*
 *  La classe TimeLine gère les images clés et la lecture de l'animation. Elle peut enregistrer jusqu'à  images clés.
 *  La durée de l'animation peut être modulée en jouant sur sa vitesse de lecture.
 */
class TimeLine
{
public:
    TimeLine (sf::Vector2f up_left, sf::Vector2f bot_right) ;

    bool IsClicked (const sf::Vector2f mouse, TreeItem* root) ;
    void MoveCursor (direction_e d, TreeItem* root) ;
    void SetPlayAnimation (bool bPlay) ;
    void StopAnimation () ;
    void Draw(sf::RenderWindow &window) ;
    void AddKeyframe (TreeItem *root, unsigned int nb_sprites) ;
    void DeleteKeyframe () ;
    void AnimationHandler (TreeItem *root) ;

    static unsigned int iquo (float dividend, float divisor) ;
    static float iremf (float dividend, float divisor) ;

    sf::CircleShape circle_origine ;

private:
    void SetKeyframePosition (KeyFrame &key) ;
    void InitialiseKeyFrame (unsigned int time, TreeItem *root, unsigned int nb_sprites) ;
    void UpdateKeyframe (KeyFrame* key, TreeItem *root) ;
    void UpdateKeyframeRecursive (KeyFrame* key, TreeItem *item) ;
    KeyFrame* GetKeyframe (unsigned int k) ;
    void GetKeyframes (float time, KeyFrame *key_1, KeyFrame *key_2) ;
    KeyFrame InterpolatePosition (float time, const KeyFrame *key_1, const KeyFrame *key_2) ;
    KeyFrame InterpolatePositionRelative (float time, const KeyFrame *key_1, const KeyFrame *key_2) ;
    void UpdateSpritesPositionsRecursive (TreeItem* item, KeyFrame *key) ;
    void UpdateSpritesPositionsRecursiveRelative (TreeItem* root, KeyFrame *key) ;
    void UpdateCursor (float time) ;

protected:
    sf::RectangleShape rect ; // Affichage de la time line.
    sf::RectangleShape rect_cursor ; // Affichage du curseur.
    unsigned int cursor ; // Position du curseur sur la time line en pourcentage.
    std::vector<KeyFrame> table_keyframes ; // Tableau de key frames rangé dans l'odre croissant de leur attribut time.
    unsigned int nb_max_keyframes ; // Nombre maximal que peut contenir la time line.
    float spacing_keyframes ; // Espace entre les keyframes sur la time line;
    float time_between_keyframes ; // Temps séparant deux key frames. En millisecondes.
    float animation_duration ; // Durée totale de l'animation.
    sf::Clock timer ;
    bool bPlayAnimation ;
    sf::Vector2f origine ; // Origine du plan dans lequel est jouée l'animation.

    unsigned int past_key_1 ;
    float past_time ;
};

#endif // _H_TIMELINE_
