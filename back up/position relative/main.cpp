/*
    All rights reserved to Baboo
    -------------------------------------------------------------------------------------------
    Ce programme sert à réaliser des animations en 2D par images clés.
    -------------------------------------------------------------------------------------------
    Mai 2015
*/

#include <Graphics.hpp>
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

#define MENU_HEIGHT 20
#define SPRITE_TREE_WIDTH 275
#define TIME_LINE_HEIGHT 50
#define CONTROL_PANEL_HEIGHT 40

#define MARGIN_RECT 2
using namespace std ;

map<string, sf::Texture> global_textures ;

void LoadTextures () ;

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

    PushButton time_line_add_keyframe ("Ajouter", x_time_line + w_time_line - 130, y_time_line) ;
    PushButton time_line_delete_keyframe ("Supprimer", x_time_line + w_time_line - 130, y_time_line + 25) ;

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
    CalculateItemsPosition(root, x_tree + MARGIN_RECT, y_tree) ;
    InitialiseSpritesPosition(&sprites_displaying, x_animation, y_animation, w_animation, h_animation) ;
    time_line.AddKeyframe(root, sprites_displaying.size()) ;

    /*  ____________________________________________________________
        COMMANDES
        ____________________________________________________________
        G : Tanslater
        R : Faire pivoter
        Clic droit : Placer le centre de gravité
        W : Répercuter les transformations du père sur ses fils
        + : Remonter le sprite dans la pile de dessin.
        - : Descendre le sprite dans la pile de dessin.
        Left : Déplacer le curseur de la time line sur la gauche.
        Right : Déplacer le curseur de la time line sur la droite.
     */

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

    bool bTranslateSprite = false ;
    bool bRotateSprite = false ;
    bool bTransformationsWithChildren = false ;

    sprite_t *selected_sprite = NULL ;
    bool bReadAnimation = false ;

    sf::Vector2f mouse_pos_last_cycle = sf::Vector2f(0,0) ;
    double angle_last_cycle = 0 ; /* En radians. */

    selected_sprite = &root->m_sprite ;
    sf::CircleShape circle(1) ;
    circle.setFillColor(sf::Color::Red) ; /* Marque le centre de gravité du sprite sélectionné. */
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close() ;

            /// Gestion de la souris.
            sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y) ;
            bLeftMouseButtonPressedNow = sf::Mouse::isButtonPressed(sf::Mouse::Left) ;
            bRightMouseButtonPressedNow = sf::Mouse::isButtonPressed(sf::Mouse::Right) ;

            if (bLeftMouseButtonPressedNow && !bLeftMouseButtonPressedLastCycle) /// Clic gauche
            {
                bLeftMouseButtonPressedLastCycle = true ;

                if (rect_animation.getGlobalBounds().contains(mouse) && !bTranslateSprite && !bRotateSprite)
                    SpritesClicHandler(&sprites_displaying, mouse, root) ;
                else if (rect_time_line.getGlobalBounds().contains(mouse))
                {
                    if (time_line.IsClicked(mouse, root)) {}
                    else if (time_line_add_keyframe.isClicked(mouse) && !bReadAnimation) time_line.AddKeyframe(root, sprites_displaying.size()) ;
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
                        }
                        else // On met l'animation en pause.
                        {
                            control_play.SetTexture(global_textures["play.png"]);
                            control_backward.SetClickable(true) ;
                            control_forward.SetClickable(true) ;

                            bTranslateSprite = false ;
                            bRotateSprite = false ;
                            control_rotate.SetTexture(global_textures["rotate_off.png"]) ;
                            control_translate.SetTexture(global_textures["translate_off.png"]) ;
                        }
                    }
                    else if (control_stop.isClicked(mouse)) /* BOUTON STOP */
                    {
                        if (bReadAnimation) // On stoppe l'animation et on la remet à zéro.
                        {
                            bReadAnimation = false ;
                            time_line.SetPlayAnimation(bReadAnimation) ;

                            bTranslateSprite = false ;
                            bRotateSprite = false ;
                            control_rotate.SetTexture(global_textures["rotate_off.png"]) ;
                            control_translate.SetTexture(global_textures["translate_off.png"]) ;

                            control_play.SetTexture(global_textures["play.png"]);
                            control_backward.SetClickable(true) ;
                            control_forward.SetClickable(true) ;
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
            else if (!bLeftMouseButtonPressedNow && bLeftMouseButtonPressedLastCycle) // Détection du relâchement du bouton gauche de la souris.
                bLeftMouseButtonPressedLastCycle = false ;

            if (bRightMouseButtonPressedNow && !bRightMouseButtonPressedLastCycle && selected_sprite != NULL) /// Clic droit : Placer le centre de gravité
            {
                bRightMouseButtonPressedLastCycle = true ;
                if (!bTranslateSprite && !bRotateSprite) // On ne peut placer le centre de gravité du sprite que s'il est à l'arrêt.
                    SetNewOrigin(selected_sprite, mouse) ;
            }
            else if (!bRightMouseButtonPressedNow && bRightMouseButtonPressedLastCycle)
                bRightMouseButtonPressedLastCycle = false ;

            /// Gestion du clavier.
            bGPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::G) ;
            bRPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::R) ;
            bWPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::W) ;
            bPlusPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Add) ;
            bMoinsPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) ;
            bLeftPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ;
            bRightPressedNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ;

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

            if (bWPressedNow && !bWPressedLastCycle) /* W : Répercuter les transformations du père sur ses fils (duplication dans "Gestion souris -> BOUTON LINK"). */
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

            if (bPlusPressedNow && !bPlusPressedLastCycle) /* + : Remonter le sprite sélectionné dans la pile de dessin. */
            {
                bPlusPressedLastCycle = true ;

                if (selected_sprite != NULL)
                    MoveSpriteUp (selected_sprite, sprites_displaying) ;
            }
            else if (!bPlusPressedNow && bPlusPressedLastCycle)
                bPlusPressedLastCycle = false ;

            if (bMoinsPressedNow && !bMoinsPressedLastCycle) /* - : Descendre le sprite sélectionné dans la pile de dessin. */
            {
                bMoinsPressedLastCycle = true ;

                if (selected_sprite != NULL)
                    MoveSpriteDown (selected_sprite, sprites_displaying) ;
            }
            else if (!bMoinsPressedNow && bMoinsPressedLastCycle)
                bMoinsPressedLastCycle = false ;

            if (bLeftPressedNow && !bLeftPressedLastCycle) /* Left : Déplacer le curseur de la time line sur la gauche. */
            {
                bLeftPressedLastCycle = true ;

                time_line.MoveCursor(LEFT, root) ;
            }
            else if (!bLeftPressedNow && bLeftPressedLastCycle)
                bLeftPressedLastCycle = false ;

            if (bRightPressedNow && !bRightPressedLastCycle) /* Right : Déplacer le curseur de la time line sur la droite. */
            {
                bRightPressedLastCycle = true ;

                time_line.MoveCursor(RIGHT, root) ;
            }
            else if (!bRightPressedNow && bRightPressedLastCycle)
                bRightPressedLastCycle = false ;

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
                        RotateWithChildren(selected_sprite->tree_item, selected_sprite->sprite.getPosition(), d_angle, 0) ;
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
        }

        if (bReadAnimation)
            time_line.AnimationHandler(root) ;

        window.clear(sf::Color::White) ;

        /// Affichage des différentes zones de la fenêtre.
        window.draw(rect_menu) ;

        window.draw(rect_tree) ;
        DrawTree(&window, root) ; // On dessine l'arbre.

        window.draw(rect_time_line) ;
        time_line.Draw(window) ;
        time_line_add_keyframe.Draw(&window) ;
        time_line_delete_keyframe.Draw(&window) ;

        window.draw(rect_animation) ;
        DrawAnimationSprites(&window, &sprites_displaying) ; // On dessine les sprites dans la fenêtre d'animation.
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

        window.display() ;
    }

    BurnTree(root) ;

    return 0;
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
        system("pause") ;
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
            system("pause") ;
            exit(-1) ;
        }

        global_textures[line] = texture ;
    }

    file.close() ;
}
