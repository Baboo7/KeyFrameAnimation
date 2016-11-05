#include "GUI.hpp"

/// DEBUGGING
#include <iostream>

/// ////////////////////
///
/// LABEL
///
/// ////////////////////

void Label::InitializeAttributes ()
{
    if (!font.loadFromFile("ressources/polices/Eligible.ttf"))
    {
        std::cout << "ERREUR: Impossible de charger la police Eligible.ttf" << std::endl ;
        system("pause") ;
        exit(-1) ;
    }

    c_normal = sf::Color::White ;
}

Label::Label (sf::String s, int X, int Y)
{
    InitializeAttributes () ;

    hasText = true ;
    hasTexture = false ;

    text.setCharacterSize(18) ;
    text.setFont(font) ;
    text.setColor(c_normal) ;
    SetPosition(X, Y) ;
    text.setString(s) ;
}

Label::Label (const sf::Texture &n_texture, int X, int Y)
{
    InitializeAttributes () ;

    hasText = false ;
    hasTexture = true ;

    sprite.setTexture (n_texture) ;
    SetPosition(X, Y) ;
}

Label::Label (sf::String n_text, const sf::Texture &n_texture, int X, int Y)
{
    InitializeAttributes () ;

    hasText = true ;
    hasTexture = true ;

    text.setCharacterSize(18) ;
    text.setFont(font) ;
    text.setColor(c_normal) ;
    text.setString(n_text) ;

    sprite.setTexture(n_texture) ;

    SetPosition(X, Y) ;
}

Label::~Label ()
{

}

void Label::Draw (sf::RenderWindow *w)
{
    if (hasTexture)
        w->draw(sprite) ;

    if (hasText)
        w->draw(text) ;
}

void Label::SetTexture (const sf::Texture &n_texture)
{
    sprite.setTexture(n_texture) ;

    hasTexture = true ;
}

/* Le comportement de SetPosition dépend des booléens hasTexture et hasText */
void Label::SetPosition (int X, int Y)
{
    if (hasTexture)
    {
        // Le sprite est affiché avant le texte.
        sprite.setPosition(X, Y) ;

        if (hasText)
            text.setPosition(X + sprite.getLocalBounds().width + MARGIN, Y) ;
    }
    else if (hasText)
    {
        text.setPosition(X, Y) ;
    }
}

void Label::SetText (sf::String s)
{
    text.setString(s) ;
}

/// ////////////////////
///
/// PUSH BUTTON
///
/// ////////////////////

void PushButton::InitializeAttributes()
{
    bClickable = true ;
    bSelected = false ;

    c_selected.r = 153 ;
    c_selected.g = 153 ;
    c_selected.b = 255 ;
}

PushButton::PushButton (sf::String s, int X, int Y) : Label (s, X, Y)
{
    InitializeAttributes() ;
}

PushButton::PushButton (const sf::Texture &n_texture, int X, int Y) : Label (n_texture, X, Y)
{
    InitializeAttributes() ;
}

PushButton::PushButton (sf::String n_text, const sf::Texture &n_texture, int X, int Y) : Label (n_text, n_texture, X, Y)
{
    InitializeAttributes() ;
}

PushButton::~PushButton ()
{

}

/*
   Le comportement de isClicked dépend du booléen bClickable.
   Si le bouton est désactivé, il n'est pas cliquable.
 */
bool PushButton::isClicked (int X, int Y)
{
    return isClicked(sf::Vector2f(X,Y)) ;
}

/*
   Le comportement de isClicked dépend du booléen bClickable.
   Si le bouton est désactivé, il n'est pas cliquable.
 */
bool PushButton::isClicked (sf::Vector2f m)
{
    if (!bClickable)
        return false ;

    bool bTexture = false, bText = false ;
    if (hasTexture)
        bTexture = sprite.getGlobalBounds().contains(m) ;

    if (hasText)
        bText = text.getGlobalBounds().contains(m) ;

    return (bText + bTexture) ;
}

void PushButton::SetClickable (bool b)
{
    bClickable = b ;

    if (bClickable && hasTexture)
    {
        sprite.setColor(sf::Color::White) ;
    }
    else if (!bClickable && hasTexture)
    {
        sprite.setColor(sf::Color::Black) ; // On color le sprite en rouge s'il n'est pas clickable.
    }
}

void PushButton::SetSelected (bool b)
{
    bSelected = b ;

    if (hasText)
    {
        if (bSelected)
        {
            text.setColor(c_selected) ;
        }
        else
        {
            text.setColor(c_normal) ;
        }
    }
}
















