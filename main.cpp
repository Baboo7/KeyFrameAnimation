/*
    All rights reserved to Baboo
    -------------------------------------------------------------------------------------------
    Ce programme sert � r�aliser des animations en 2D par images cl�s.
    -------------------------------------------------------------------------------------------
    Mai 2015
*/

#include <SFML/Graphics.hpp>
#include <math.h>
#include "GUI.hpp"
#include "tree.hpp"
#include "sprite_manager.hpp"
#include "time_line.hpp"
#include <fstream>

/// DEBUGGING
#include <iostream>


#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 800

#define MENU_HEIGHT 25
#define SPRITE_TREE_WIDTH 275
#define TIME_LINE_HEIGHT 50
#define CONTROL_PANEL_HEIGHT 40

#define MARGIN_RECT 2
using namespace std ;

map<string, sf::Texture> global_textures ;

void LoadTextures () ;
void rSplitString (vector<string> &table_s,  const char &separator, string to_split) ;

void GenerateAnimation (float animation_duration , float time_between_keyframes , unsigned int nb_max_keyframes , sf::Vector2f origine , vector<sprite_t *> &sprites_displaying , std::vector<KeyFrame>* table_keyframes) ;
void GenerateSprite (TreeItem *root , vector<sprite_t *> &sprite_displaying) ;

TreeItem* LoadSprite (unsigned int &nb_sprites, vector<sprite_t *> &sprite_displaying) ;
bool LoadAnimation (unsigned int nb_sprites, float &new_anim_duration, unsigned int &new_nb_max_key, float &new_time_between_key, sf::Vector2f &new_origine, vector<KeyFrame> &table_keys) ;

int main()
{
    LoadTextures() ;

    sf::Color c_rect ;
        c_rect.r = 200 ;
        c_rect.g = 200 ;
        c_rect.b = 200 ;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Key Frame Animator");

    /// ////////////////
    /// MENU
    /// ////////////////
    sf::RectangleShape rect_menu (sf::Vector2f(WINDOW_WIDTH - 2 * MARGIN_RECT, MENU_HEIGHT)) ;
    rect_menu.setPosition (MARGIN_RECT, MARGIN_RECT) ;
    rect_menu.setFillColor (c_rect) ;

    ComboBox combo_file ("Fichier", MARGIN_RECT + MARGIN, MARGIN_RECT) ;
    combo_file.AddPushButton("Nouveau") ;
    combo_file.AddPushButton("Ouvrir") ;
    combo_file.AddPushButton("G�n�rer") ;
    combo_file.AddPushButton("Quitter") ;

    /// ////////////////
    /// SPRITES TREE
    /// ////////////////
    int x_tree = MARGIN_RECT ;
    int y_tree = MENU_HEIGHT + 2 * MARGIN_RECT ;
    int w_tree = SPRITE_TREE_WIDTH ;
    int h_tree = WINDOW_HEIGHT - (3 * MARGIN_RECT + MENU_HEIGHT) ;
    sf::RectangleShape rect_tree (sf::Vector2f(w_tree, h_tree)) ;
    rect_tree.setPosition (x_tree, y_tree) ;
    rect_tree.setFillColor (c_rect) ;

    /// ////////////////
    /// TIME LINE
    /// ////////////////
    int x_time_line = SPRITE_TREE_WIDTH + 2 * MARGIN_RECT ;
    int y_time_line = MENU_HEIGHT + 2 * MARGIN_RECT ;
    int w_time_line = WINDOW_WIDTH - (SPRITE_TREE_WIDTH + 3 * MARGIN_RECT) ;
    int h_time_line = TIME_LINE_HEIGHT ;
    sf::RectangleShape rect_time_line (sf::Vector2f(w_time_line, h_time_line)) ;
    rect_time_line.setPosition (x_time_line, y_time_line) ;
    rect_time_line.setFillColor (c_rect) ;
    TimeLine time_line (sf::Vector2f(x_time_line, y_time_line), sf::Vector2f(x_time_line + 700, y_time_line + h_time_line)) ;

    PushButton time_line_add_keyframe ("Ajouter", x_time_line + w_time_line - 110, y_time_line) ;
    PushButton time_line_delete_keyframe ("Supprimer", x_time_line + w_time_line - 110, y_time_line + 25) ;

    Label time_line_label_duration ("Dur�e de l'animation (ms)" , x_time_line + w_time_line - 390 , y_time_line) ;
    InputNumber time_line_duration (sf::Vector2f(x_time_line + w_time_line - 390 , y_time_line + 25), "2000", 500, 10000) ;

    /// ////////////////
    /// CONTROL PANEL
    /// ////////////////
    int x_control_panel = SPRITE_TREE_WIDTH + 2 * MARGIN_RECT ;
    int y_control_panel = WINDOW_HEIGHT - (CONTROL_PANEL_HEIGHT + MARGIN_RECT) ;
    int w_control_panel = WINDOW_WIDTH - (SPRITE_TREE_WIDTH + 3 * MARGIN_RECT) ;
    int h_control_panel = CONTROL_PANEL_HEIGHT ;
    sf::RectangleShape rect_control_panel (sf::Vector2f(w_control_panel, h_control_panel)) ;
    rect_control_panel.setPosition (x_control_panel, y_control_panel) ;
    rect_control_panel.setFillColor (c_rect) ;

    PushButton control_backward(global_textures["backward.png"], x_control_panel + 20, y_control_panel) ;
    PushButton control_stop(global_textures["stop.png"], x_control_panel + 105, y_control_panel) ;
    PushButton control_play(global_textures["play.png"], x_control_panel + 155, y_control_panel) ;
    PushButton control_forward(global_textures["forward.png"], x_control_panel + 230, y_control_panel) ;

    PushButton control_link(global_textures["link_off.png"], x_control_panel + w_control_panel - 2*(40 + 5), y_control_panel) ;
    Label control_rotate(global_textures["rotate_off.png"], x_control_panel + w_control_panel - 4*(40 + 5), y_control_panel) ;
    Label control_translate(global_textures["translate_off.png"], x_control_panel + w_control_panel - 5*(40 + 5), y_control_panel) ;

    /// ////////////////
    /// ANIMATION
    /// ////////////////
    int x_animation = SPRITE_TREE_WIDTH + 2 * MARGIN_RECT ;
    int y_animation = MENU_HEIGHT + TIME_LINE_HEIGHT + 3 * MARGIN_RECT ;
    int w_animation = WINDOW_WIDTH - (SPRITE_TREE_WIDTH + 3 * MARGIN_RECT) ;
    int h_animation = WINDOW_HEIGHT - (MENU_HEIGHT + TIME_LINE_HEIGHT + CONTROL_PANEL_HEIGHT + 5 * MARGIN_RECT) ;
    sf::RectangleShape rect_animation (sf::Vector2f(w_animation, h_animation)) ;
    rect_animation.setPosition (x_animation, y_animation) ;
    rect_animation.setFillColor (c_rect) ;

    vector<sprite_t *> sprites_displaying ;
    TreeItem* root = BuildTree(&sprites_displaying) ;
    CalculateItemsPositionOnTreeRect (root, x_tree + MARGIN_RECT, y_tree) ;
    InitialiseSpritesPositionOnAnimationRect (&sprites_displaying, x_animation, y_animation, w_animation, h_animation) ;
    time_line.AddKeyframe(sprites_displaying) ;

    /** _____________________________________________________________
        COMMANDES
        ____________________________________________________________
        G : Tanslater
        R : Faire pivoter
        Clic droit : Placer le centre de gravit�
        W : R�percuter les transformations du p�re sur ses fils
        + : Remonter le sprite dans la pile de dessin.
        - : Descendre le sprite dans la pile de dessin.
        Left : D�placer le curseur de la time line sur la gauche.
        Right : D�placer le curseur de la time line sur la droite.
        O : Placer l'origine de l'animation.
        U : Demander l'actualisation de la position des sprites ou non lorsque la position du curseur est modifi�e.
    **/

    bool bLeftMouseButtonPressedLastCycle = false ;
    bool bLeftMouseButtonPressedNow = false ;

    bool bGPressedLastCycle = false ;
    bool bGPressedNow = false ;
    bool bRPressedLastCycle = false ;
    bool bRPressedNow = false ;
    bool bRightMouseButtonPressedLastCycle = false ;
    bool bRightMouseButtonPressedNow = false ;
    bool bWPressedLastCycle = false ;
    bool bWPressedNow = false ;
    bool bPlusPressedLastCycle = false ;
    bool bPlusPressedNow = false ;
    bool bMoinsPressedLastCycle = false ;
    bool bMoinsPressedNow = false ;
    bool bLeftPressedLastCycle = false ;
    bool bLeftPressedNow = false ;
    bool bRightPressedLastCycle = false ;
    bool bRightPressedNow = false ;
    bool bOPressedLastCycle = false ;
    bool bOPressedNow = false ;
    bool bUPressedLastCycle = false ;
    bool bUPressedNow = false ;

    bool bTranslateSprite = false ;
    bool bRotateSprite = false ;
    bool bTransformationsWithChildren = false ;

    sprite_t *selected_sprite = NULL ;
    bool bReadAnimation = false ;

    sf::Vector2f mouse_pos_last_cycle = sf::Vector2f(0,0) ;
    double angle_last_cycle = 0 ; /* En radians. */

    selected_sprite = &root->m_sprite ;
    time_line.SetAnimationOrigine(root->m_sprite.sprite.getPosition()) ;
    sf::CircleShape circle(1) ;
    circle.setFillColor(sf::Color::Red) ; /* Marque le centre de gravit� du sprite s�lectionn�. */
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close() ;

            /// //////////////////
            /// Gestion de la souris.
            /// //////////////////

            sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) ;
            bLeftMouseButtonPressedNow = sf::Mouse::isButtonPressed(sf::Mouse::Left) ;
            bRightMouseButtonPressedNow = sf::Mouse::isButtonPressed(sf::Mouse::Right) ;

            /// //////////////////
            /// Gestion du survolage.
            /// //////////////////

            combo_file.HoverHandler(mouse) ;
            time_line_add_keyframe.HoverHandler(mouse) ;
            time_line_delete_keyframe.HoverHandler(mouse) ;

            /// //////////////////
            /// Gestion des boutons de la souris.
            /// //////////////////

            if (bLeftMouseButtonPressedNow && !bLeftMouseButtonPressedLastCycle) /// Clic gauche.
            {
                bLeftMouseButtonPressedLastCycle = true ;

                int result_combo_file  = combo_file.ClicHandler(mouse) ;

                if (result_combo_file == 0) {} // Fichier -> Nouveau.
                else if (result_combo_file == 1) // Fichier -> Ouvrir.
                {
                    // Chargement du sprite.
                    unsigned int nb_sprites = 0 ;
                    vector<sprite_t *> new_sprites_displaying ;
                    TreeItem *new_root = LoadSprite(nb_sprites, new_sprites_displaying) ;
                    if (new_root == NULL)
                    {
                        for (unsigned int i = 0 ; i < new_sprites_displaying.size() ; i ++) delete new_sprites_displaying[i]->tree_item ; // On d�salloue la m�moire r�serv�e aux Treeitems.
                    }
                    else
                    {
                        // Chargement de l'animation.
                        float new_anim_duration = 0.0f ;
                        unsigned int new_nb_max_key = 0 ;
                        float new_time_between_key = 0.0f ;
                        sf::Vector2f new_origine = sf::Vector2f(0.0f , 0.0f) ;
                        vector<KeyFrame> table_keys ;
                        if (LoadAnimation(nb_sprites, new_anim_duration, new_nb_max_key, new_time_between_key, new_origine, table_keys))
                        {
                            for (unsigned int i = 0 ; i < new_sprites_displaying.size() ; i ++) delete new_sprites_displaying[i]->tree_item ; // On d�salloue la m�moire r�serv�e aux Treeitems.
                        }
                        else
                        {
                            sprites_displaying = new_sprites_displaying ;
                            BurnTree(root) ; // On d�salloue la m�moire r�serv�e au sprite pr�c�dent.

                            InitialiseSpritesPositionOnAnimationRect (&sprites_displaying, x_animation, y_animation, w_animation, h_animation) ;

                            root = new_root ; // On utilise le nouveau sprite.
                            CalculateItemsPositionOnTreeRect (root, x_tree + MARGIN_RECT, y_tree) ;
                            selected_sprite = &root->m_sprite ;

                            time_line.ReinitTimeLine(new_anim_duration,
                                                     new_nb_max_key,
                                                     new_time_between_key,
                                                     new_origine,
                                                     table_keys,
                                                     root) ;

                            time_line_duration.SetValue((int) new_anim_duration) ;
                        }
                    }
                }
                else if (result_combo_file == 2) // Fichier -> G�n�rer.
                {
                    GenerateAnimation(time_line.GetAnimationDuration() ,
                                      time_line.GetTimeBetweenKeyframes() ,
                                      time_line.GetNbMaxKeyframes() ,
                                      time_line.GetAnimationOrigine () ,
                                      sprites_displaying ,
                                      time_line.GetTableKeyframes()) ;
                    GenerateSprite(root,
                                   sprites_displaying) ;
                }
                else if (result_combo_file == 3) window.close() ; // Fichier -> Quitter.
                else if (time_line_duration.ClicHandler(mouse)) {}
                else if (rect_animation.getGlobalBounds().contains(mouse) && !bTranslateSprite && !bRotateSprite) SpritesClicHandler(&sprites_displaying, mouse, root) ;
                else if (rect_time_line.getGlobalBounds().contains(mouse))
                {
                    if (time_line.IsClicked(mouse, root)) {}
                    else if (time_line_add_keyframe.isClicked(mouse) && !bReadAnimation) time_line.AddKeyframe(sprites_displaying) ;
                    else if (time_line_delete_keyframe.isClicked(mouse)&& !bReadAnimation) time_line.DeleteKeyframe() ;
                }
                else if (rect_tree.getGlobalBounds().contains(mouse) && !bTranslateSprite && !bRotateSprite) TreeClicHandler(root, mouse, &sprites_displaying) ;
                else if (rect_control_panel.getGlobalBounds().contains(mouse))
                {
                    if (control_play.isClicked(mouse)) /* BOUTON PLAY */
                    {
                        bReadAnimation = !bReadAnimation ;
                        time_line.SetPlayAnimation(bReadAnimation) ;

                        if (bReadAnimation) // On lance la lecture de l'animation.
                        {
                            control_play.SetTexture(global_textures["pause.png"]);
                            control_backward.SetClickable(false) ;
                            control_forward.SetClickable(false) ;
                            combo_file.SetEnabled(false) ;
                        }
                        else // On met l'animation en pause.
                        {
                            control_play.SetTexture(global_textures["play.png"]);
                            control_backward.SetClickable(true) ;
                            control_forward.SetClickable(true) ;
                            combo_file.SetEnabled(true) ;

                            bTranslateSprite = false ;
                            bRotateSprite = false ;
                            control_rotate.SetTexture(global_textures["rotate_off.png"]) ;
                            control_translate.SetTexture(global_textures["translate_off.png"]) ;
                        }
                    }
                    else if (control_stop.isClicked(mouse)) /* BOUTON STOP */
                    {
                        time_line.StopAnimation(root) ;

                        if (bReadAnimation) // On stoppe l'animation et on la remet � z�ro.
                        {
                            bReadAnimation = false ;

                            bTranslateSprite = false ;
                            bRotateSprite = false ;
                            control_rotate.SetTexture(global_textures["rotate_off.png"]) ;
                            control_translate.SetTexture(global_textures["translate_off.png"]) ;
                            control_play.SetTexture(global_textures["play.png"]);
                            control_backward.SetClickable(true) ;
                            control_forward.SetClickable(true) ;
                            combo_file.SetEnabled(true) ;
                        }
                    }
                    else if (control_link.isClicked(mouse)) /* BOUTON LINK (duplication dans "Gestion clavier -> W") */
                    {
                        bTransformationsWithChildren = !bTransformationsWithChildren ;

                        if (bTransformationsWithChildren) control_link.SetTexture(global_textures["link_on.png"]) ;
                        else control_link.SetTexture(global_textures["link_off.png"]) ;
                    }
                    /* BOUTON BACKWARD */
                    else if (control_backward.isClicked(mouse)) time_line.MoveCursor(LEFT, root) ;
                    /* BOUTON FORWARD */
                    else if (control_forward.isClicked(mouse)) time_line.MoveCursor(RIGHT, root) ;
                }

                selected_sprite = GetSelectedSprite(&sprites_displaying) ;
            }
            else if (!bLeftMouseButtonPressedNow && bLeftMouseButtonPressedLastCycle) // D�tection du rel�chement du bouton gauche de la souris.
                bLeftMouseButtonPressedLastCycle = false ;

            if (bRightMouseButtonPressedNow && !bRightMouseButtonPressedLastCycle && selected_sprite != NULL) /// Clic droit : Placer le centre de gravit�
            {
                bRightMouseButtonPressedLastCycle = true ;
                if (!bTranslateSprite && !bRotateSprite) // On ne peut placer le centre de gravit� du sprite que s'il est � l'arr�t.
                    SetNewOrigin(selected_sprite, mouse) ;
            }
            else if (!bRightMouseButtonPressedNow && bRightMouseButtonPressedLastCycle)
                bRightMouseButtonPressedLastCycle = false ;

            /// ///////////////////
            /// Gestion du clavier.
            /// ///////////////////

            bGPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::G) ;
            bRPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::R) ;
            bWPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::W) ;
            bPlusPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Add) ;
            bMoinsPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) ;
            bLeftPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ;
            bRightPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ;
            bOPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::O) ;
            bUPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::U) ;

            /* Si un input est actif, il monopolise toutes les entr�es clavier. */
            if (time_line_duration.KeyboardHandler(event)) {}
            else
            {
                if (bGPressedNow && !bGPressedLastCycle && selected_sprite != NULL) /* G : TRANSLATER. */
                {
                    bGPressedLastCycle = true ;
                    bTranslateSprite = !bTranslateSprite ;

                    if (bTranslateSprite)
                    {
                        bRotateSprite = false ;
                        control_rotate.SetTexture(global_textures["rotate_off.png"]) ;
                        control_translate.SetTexture(global_textures["translate_on.png"]) ;
                    }
                    else
                        control_translate.SetTexture(global_textures["translate_off.png"]) ;
                }
                else if (!bGPressedNow && bGPressedLastCycle)
                    bGPressedLastCycle = false ;

                if (bRPressedNow && !bRPressedLastCycle && selected_sprite != NULL) /* R : FAIRE PIVOTER. */
                {
                    bRPressedLastCycle = true ;
                    bRotateSprite = !bRotateSprite ;

                    if (bRotateSprite)
                    {
                        bTranslateSprite = false ;
                        control_translate.SetTexture(global_textures["translate_off.png"]) ;
                        control_rotate.SetTexture(global_textures["rotate_on.png"]) ;
                    }
                    else
                        control_rotate.SetTexture(global_textures["rotate_off.png"]) ;
                }
                else if (!bRPressedNow && bRPressedLastCycle)
                    bRPressedLastCycle = false ;

                if (bWPressedNow && !bWPressedLastCycle) /* W : R�percuter les transformations du p�re sur ses fils (duplication dans "Gestion souris -> BOUTON LINK"). */
                {
                    bWPressedLastCycle = true ;
                    bTransformationsWithChildren = !bTransformationsWithChildren ;

                    if (bTransformationsWithChildren)
                        control_link.SetTexture(global_textures["link_on.png"]) ;
                    else
                        control_link.SetTexture(global_textures["link_off.png"]) ;
                }
                else if (!bWPressedNow && bWPressedLastCycle)
                    bWPressedLastCycle = false ;

                if (bPlusPressedNow && !bPlusPressedLastCycle) /* + : Remonter le sprite s�lectionn� dans la pile de dessin. */
                {
                    bPlusPressedLastCycle = true ;

                    if (selected_sprite != NULL)
                        MoveSpriteUp (selected_sprite, sprites_displaying) ;
                }
                else if (!bPlusPressedNow && bPlusPressedLastCycle)
                    bPlusPressedLastCycle = false ;

                if (bMoinsPressedNow && !bMoinsPressedLastCycle) /* - : Descendre le sprite s�lectionn� dans la pile de dessin. */
                {
                    bMoinsPressedLastCycle = true ;

                    if (selected_sprite != NULL)
                        MoveSpriteDown (selected_sprite, sprites_displaying) ;
                }
                else if (!bMoinsPressedNow && bMoinsPressedLastCycle)
                    bMoinsPressedLastCycle = false ;

                if (bLeftPressedNow && !bLeftPressedLastCycle) /* Left : D�placer le curseur de la time line sur la gauche. */
                {
                    bLeftPressedLastCycle = true ;

                    time_line.MoveCursor(LEFT, root) ;
                }
                else if (!bLeftPressedNow && bLeftPressedLastCycle)
                    bLeftPressedLastCycle = false ;

                if (bRightPressedNow && !bRightPressedLastCycle) /* Right : D�placer le curseur de la time line sur la droite. */
                {
                    bRightPressedLastCycle = true ;

                    time_line.MoveCursor(RIGHT, root) ;
                }
                else if (!bRightPressedNow && bRightPressedLastCycle)
                    bRightPressedLastCycle = false ;

                if (bOPressedNow && !bOPressedLastCycle && rect_animation.getGlobalBounds().contains(mouse)) /* O : Placer l'origine de l'animation. */
                {
                    bOPressedLastCycle = true ;

                    time_line.SetAnimationOrigine (mouse) ;
                }
                else if (!bOPressedNow && bOPressedLastCycle)
                    bOPressedLastCycle = false ;

                if (bUPressedNow && !bUPressedLastCycle) /* U : Actualisation de la position des sprites. */
                {
                    bUPressedLastCycle = true ;

                    time_line.ToggleUpdateSpritesPositionOnCursorMove () ;
                }
                else if (!bUPressedNow && bUPressedLastCycle)
                    bUPressedLastCycle = false ;
            }

            int new_duration = 0 ;
            if (time_line_duration.GetValueIfUpdated(new_duration)) time_line.SetAnimationDuration((float) new_duration) ;

            if (selected_sprite != NULL && rect_animation.getGlobalBounds().contains(mouse) && !bReadAnimation  )
            {
                if (bTranslateSprite or (bLeftMouseButtonPressedNow and bLeftMouseButtonPressedLastCycle))
                {
                    sf::Vector2f translation = mouse - mouse_pos_last_cycle ;
                    if (bTransformationsWithChildren)
                    {
                        TranslateWithChildren(selected_sprite->tree_item, translation) ;
                    }
                    else
                        TranslateWithoutChildren(selected_sprite->tree_item, translation) ;
                }
                else if (bRotateSprite)
                {
                    sf::Vector2f spr_pos = selected_sprite->sprite.getPosition() ;
                    double angle = atan2(mouse.y - spr_pos.y,mouse.x - spr_pos.x) ;
                    double d_angle = (angle - angle_last_cycle)*180.f/PI ;
                    if (bTransformationsWithChildren)
                    {
                        RotateWithChildren(selected_sprite->tree_item, selected_sprite->sprite.getPosition(), d_angle) ;
                    }
                    else
                    {
                        RotateWithouthChildren(selected_sprite->tree_item, d_angle) ;
                    }
                }
            }

            mouse_pos_last_cycle = mouse ;
            if (selected_sprite != NULL)
            {
                sf::Vector2f spr_pos = selected_sprite->sprite.getPosition() ;
                angle_last_cycle = atan2(mouse.y - spr_pos.y,mouse.x - spr_pos.x) ;
            }
        } /* Fin de la gestion des évènements. */

        if (bReadAnimation)
            time_line.AnimationHandler(root) ;

        window.clear(sf::Color::White) ;

        /// Affichage des diff�rentes zones de la fen�tre.
        window.draw(rect_menu) ;

        window.draw(rect_tree) ;
        DrawTree(&window, root) ; // On dessine l'arbre.

        window.draw(rect_time_line) ;
        time_line.Draw(window) ;
        time_line_add_keyframe.Draw(&window) ;
        time_line_delete_keyframe.Draw(&window) ;
        time_line_label_duration.Draw(&window) ;
        time_line_duration.Draw(&window) ;

        window.draw(rect_animation) ;
        DrawAnimationSprites(&window, &sprites_displaying) ; // On dessine les sprites dans la fen�tre d'animation.
        if (selected_sprite != NULL)
        {
            circle.setPosition(selected_sprite->sprite.getPosition()) ;
            window.draw(circle) ;
        }
        window.draw(time_line.circle_origine) ;


        window.draw(rect_control_panel) ;
        control_backward.Draw(&window) ;
        control_stop.Draw(&window) ;
        control_play.Draw(&window) ;
        control_forward.Draw(&window) ;
        control_link.Draw(&window) ;
        control_translate.Draw(&window) ;
        control_rotate.Draw(&window) ;

        combo_file.Draw(&window) ;

        window.display() ;
    }

    BurnTree(root) ;

    return 0;
}

void GenerateAnimation(float animation_duration , float time_between_keyframes , unsigned int nb_max_keyframes , sf::Vector2f origine , vector<sprite_t *> &sprites_displaying , std::vector<KeyFrame>* table_keyframes)
{
    // Ouverture du fichier.
    string folder = "./animation/" ;
    string name_file = folder + "animation.txt" ;
    ofstream file ;
    file.open(name_file.c_str() , ios::out) ;

    if (file.bad())
    {
        cout << "ERREUR: Le fichier " << name_file.c_str() << " n'a pas pu �tre cr�� correctement." << endl ;
        exit(-1) ;
    }

    // �criture dans le fichier.
    file << animation_duration << ":" << nb_max_keyframes << ":" << time_between_keyframes << endl ; // On indique la dur�e de l'animation, le nombre de keyframes maximales qui composent l'animation et le temps qui s�pare deux keyframes.
    file << origine.x << "/" << origine.y << endl ; // On inscrit la position de l'origine de l'animation.

    file << endl ; // On ajoute un saut de ligne par soucis de lisibilité.
    for (unsigned int k = 0 ; k < table_keyframes->size() ; k ++)
    {
        KeyFrame *key = &(*table_keyframes)[k] ;
        file << "@" << (float) key->time * time_between_keyframes << endl ; // On inscrit la date de la keyframe.
        for (unsigned int i = 0 ; i < sprites_displaying.size() ; i ++)
        {
            file << key->table_positions[sprites_displaying[i]->ID].position.x - origine.x << "/" << key->table_positions[sprites_displaying[i]->ID].position.y - origine.y << "/" << key->table_positions[sprites_displaying[i]->ID].rotation << endl ;
        }
    }

    file.close() ;
}

void GenerateSprite(TreeItem *root , vector<sprite_t *> &sprite_displaying)
{
    /* Ouverture du fichier. */
    string folder = "./animation/" ;
    string name_file = folder + "sprite.txt" ;
    ofstream file ;
    file.open(name_file.c_str() , ios::out) ;

    if (file.bad())
    {
        cout << "ERREUR: Le fichier " << name_file.c_str() << " n'a pas pu �tre cr�� correctement." << endl ;
        exit(-1) ;
    }

    /* Écriture dans le fichier. */
    // On spécifie d'abord l'ordre dans lequel les sprites sont affichés.
    // Plus le sprite a un indice faible dans le tableau plus haut il est affiché,
    // par conséquent son nom est inscrit dans les premiers.
    for (unsigned int k = 0 ; k < sprite_displaying.size() ; k ++)
    {
        sprite_t *s = sprite_displaying[k] ;
        file << s->tree_item->name << "/" << s->sprite.getOrigin().x << "/" << s->sprite.getOrigin().y << endl ;
    }
    file << endl ; // On saute une ligne pour s�parer les informations de natures diff�rents.

    // On pr�cise ensuite l'arbre des d�pendances.
    string dependances = "" ;
    GetDependances(root , dependances) ;
    file << dependances << endl ;

    file.close() ;
}

TreeItem* GetTreeItemByName (vector<TreeItem*> &table, string &name)
{
    for (unsigned int i = 0 ; i < table.size() ; i ++)
    {
        if (table[i]->name == name) return table[i] ;
    }

    return NULL ;
}

TreeItem* LoadSprite (unsigned int &nb_sprites, vector<sprite_t *> &new_sprites_displaying)
{
    // Ouverture du fichier.
    string name_file = "./animation/sprite.txt" ;
    ifstream file (name_file.c_str()) ;

    if (!file)
    {
        cout << "ERREUR: Le fichier " << name_file.c_str() << " n'a pas pu �tre ouvert correctement." << endl ;
        exit(-1) ;
    }

    vector<TreeItem*> table_tree_items ;

    // Lecture du fichier.
    string line = "a" ; // On ajoute '=a' car on veut juste que line ne soit pas vide.
    vector<string> table_strings ;
    while (getline(file, line) and !line.empty())
    {
        if (line.empty()) return NULL ; // Le fichier est corrompue, on retourne un pointeur nul.

        table_strings.clear() ;
        rSplitString(table_strings, '/', line) ;

        TreeItem *item = new TreeItem (table_strings[0] , nb_sprites) ; // nb_sprites joue également le rôle d'ID.
        nb_sprites ++ ;
        item->m_sprite.sprite.setOrigin(sf::Vector2f(atof(table_strings[1].c_str()) , atof(table_strings[2].c_str()))) ;

        table_tree_items.push_back(item) ;
        new_sprites_displaying.push_back(&(item->m_sprite)) ;
    }

    TreeItem *root = NULL ;
    TreeItem *father = NULL ;
    TreeItem *child = NULL ;
    line = "a" ; // On veut juste que line ne soit pas vide.
    while (getline(file, line) and !line.empty())
    {
        if (line.empty()) return NULL ; // Le fichier est corrompue, on retourne un pointeur nul.

        table_strings.clear() ;
        rSplitString(table_strings, '>', line) ;

        father = GetTreeItemByName(table_tree_items, table_strings[0]) ; // R�cup�ration du p�re.
        child = GetTreeItemByName(table_tree_items, table_strings[1]) ; // R�cup�ration du fils.
        if (father == NULL or child == NULL) return NULL ;

        if (root == NULL) root = father ;

        father->children.push_back(child) ;
    }

    file.close() ;

    return root ;
}

bool LoadAnimation(unsigned int nb_sprites, float &new_anim_duration, unsigned int &new_nb_max_key, float &new_time_between_key, sf::Vector2f &new_origine, vector<KeyFrame> &table_keys)
{
    // Ouverture du fichier.
    string name_file = "./animation/animation.txt" ;
    ifstream file (name_file.c_str()) ;

    if (!file)
    {
        cout << "ERREUR: Le fichier " << name_file << " n'a pas pu �tre ouvert correctement." << endl ;
        exit(-1) ;
    }

    // Lecture du fichier.
    string line ;
    vector<string> strings ;

    getline(file, line) ;
    rSplitString(strings, ':', line) ;
    if (strings.size() != 3)
    {
        cout << "Le fichier " << name_file << " est corrompu." << endl ;
        return true ;
    }
    new_anim_duration = atof (strings[0].c_str()) ;
    new_nb_max_key = abs (atoi (strings[1].c_str())) ;
    new_time_between_key = atof (strings[2].c_str()) ;

    strings.clear() ;
    getline(file, line) ;
    rSplitString(strings, '/', line) ;
    if (strings.size() != 2)
    {
        cout << "Le fichier " << name_file << " est corrompu." << endl ;
        return true ;
    }
    new_origine.x = atof (strings[0].c_str()) ;
    new_origine.y = atof (strings[1].c_str()) ;

    strings.clear() ; // Pr�sence d'une ligne vide dans le fichier de donn�es.
    getline(file, line) ;

    /// Chargement des keyframes.
    KeyFrame key ;
    position_t pos ;
    unsigned int i = nb_sprites ;
    bool bFirst_iteration = true ;
    while (getline(file, line))
    {
        if (line.empty())
        {
            cout << "Le fichier " << name_file << " est corrompu." << endl ;
            return true ;
        }

        if (line.c_str()[0] == '@')
        {
            if (i != nb_sprites)
            {
                cout << "ERREUR: Le nombre de sprites ne concorde pas avec celui de l'animation." << endl ;
                return true ;
            }

            if (i == nb_sprites and !bFirst_iteration) table_keys.push_back(key) ;
            else bFirst_iteration = false ;

            key.table_positions.clear() ;
            key.time = (unsigned int) (abs (atoi (line.substr(1).c_str()))/new_time_between_key) ;
            i = 0 ;
        }
        else
        {
            strings.clear() ;
            rSplitString(strings, '/', line) ;
            if (strings.size() != 3)
            {
                cout << "Le fichier " << name_file << " est corrompu." << endl ;
                return true ;
            }

            pos.position.x = atof (strings[0].c_str()) ;
            pos.position.y = atof (strings[1].c_str()) ;
            pos.position += new_origine ;
            pos.rotation = atof (strings[2].c_str()) ;

            key.table_positions.push_back(pos) ;
            i ++ ;
        }
    }
    table_keys.push_back(key) ;

    file.close() ;

    return false ;
}

void LoadTextures()
{
    // Ouverture du fichier.
    string folder_images = "./ressources/images/" ;
    string name_file = folder_images + "images.txt" ;
    ifstream file (name_file.c_str()) ;

    if (!file)
    {
        cout << "ERREUR: Le fichier " << name_file.c_str() << " n'a pas pu être ouvert correctement." << endl ;
        exit(-1) ;
    }

    // Lecture du fichier.
    string line ;
    sf::Texture texture ;
    while (getline(file, line))
    {
        if (!texture.loadFromFile(folder_images + line))
        {
            cout << "ERREUR: Impossible de charger la texture " << folder_images << line << endl ;
            exit(-1) ;
        }

        global_textures[line] = texture ;
    }

    file.close() ;
}

void rSplitString (vector<string> &table_s, const char &separator, string to_split)
{
    if (to_split.empty()) return ;

    const char *tab_char = to_split.c_str() ;
    unsigned int i = 0 ;
    while (tab_char[i] != '\0' and tab_char[i] != separator) i ++ ;

    if (i == to_split.size()) table_s.push_back(to_split) ;
    else
    {
        table_s.push_back(to_split.substr(0, i)) ;
        rSplitString(table_s, separator, to_split.substr(i+1)) ;
    }
}
