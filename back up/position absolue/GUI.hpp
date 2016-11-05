#ifndef _GUI_H_
#define _GUI_H_

#include <Graphics.hpp>

#define MARGIN 5 // Marge entre l'icone et le texte.

/// LABEL

class Label
{
public:
    Label (sf::String s, int X, int Y) ;
    Label (const sf::Texture &n_texture, int X, int Y) ;
    Label (sf::String n_text, const sf::Texture &n_texture, int X, int Y) ;
    void InitializeAttributes() ;
    ~Label () ;

    void Draw (sf::RenderWindow *w) ;
    void SetTexture (const sf::Texture &n_texture) ;
    void SetPosition (int X, int Y) ;
    void SetText (sf::String s) ;

protected:
    sf::Font font ;
    sf::Text text ;
    sf::Sprite sprite ;
    sf::Color c_normal ;

    bool hasTexture ;
    bool hasText ;
};


/// PUSH BUTTON

class PushButton : public Label
{
public:
    PushButton (sf::String s, int X, int Y) ;
    PushButton (const sf::Texture &n_texture, int X, int Y) ;
    PushButton (sf::String n_text, const sf::Texture &n_texture, int X, int Y) ;
    void InitializeAttributes() ;
    ~PushButton () ;

    bool isClicked (int X, int Y) ;
    bool isClicked (sf::Vector2f m) ;

    void SetClickable(bool b) ;
    void SetSelected (bool b) ;
    bool GetSelected() const {return bSelected ;} ;

protected:
    bool bClickable ;
    bool bSelected ;

    sf::Color c_selected ;
};

#endif // _GUI_H_
