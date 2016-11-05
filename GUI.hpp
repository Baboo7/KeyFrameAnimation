#ifndef _GUI_H_
#define _GUI_H_

#include <SFML/Graphics.hpp>
#include <vector>

#define MARGIN 5 // Marge entre l'icone et le texte.

/// ////////////////////
///
/// LABEL
///
/// ////////////////////

class Label
{
public:
    Label (sf::String s, int X, int Y) ;
    Label (const sf::Texture &n_texture, int X, int Y) ;
    Label (sf::String n_text, const sf::Texture &n_texture, int X, int Y) ;
    void InitializeAttributes() ;

    void Draw (sf::RenderWindow *w) ;
    void SetTexture (const sf::Texture &n_texture) ;
    void SetPosition (int X, int Y) ;
    void SetText (sf::String s) ;
    void SetVisible (bool visible) { bVisible = visible ; } ;
    sf::Text GetText () const { return text ; } ;

protected:
    sf::Font font ;
    sf::Text text ;
    sf::Sprite sprite ;
    sf::Color c_normal ;

    bool hasTexture ;
    bool hasText ;

    bool bVisible ;
};

/// ////////////////////
///
/// PUSH BUTTON
///
/// ////////////////////

class PushButton : public Label
{
public:
    PushButton (sf::String s, int X, int Y) ;
    PushButton (const sf::Texture &n_texture, int X, int Y) ;
    PushButton (sf::String n_text, const sf::Texture &n_texture, int X, int Y) ;
    void InitializeAttributes() ;

    bool isClicked (int X, int Y) ;
    bool isClicked (sf::Vector2f m) ;
    void HoverHandler (sf::Vector2f mouse) ;

    void SetClickable(bool b) ;
    void SetSelected (bool b) ;
    bool GetSelected() const {return bSelected ;} ;

protected:
    bool bClickable ;
    bool bSelected ;
    bool bHovered_last_cycle ;

    sf::Color c_selected ;
    sf::Color c_hovered ;
};


/// ////////////////////
///
/// COMBO BOX
///
/// ////////////////////

class ComboBox
{
public:
    ComboBox (const sf::Texture &tex, sf::String text, int X, int Y) ;
    ComboBox (sf::String text, int X, int Y) ;
    ~ComboBox() ;
    void Draw (sf::RenderWindow *window) ;
    void HoverHandler (sf::Vector2f mouse) ;
    void AddPushButton (sf::String text) ;
    int ClicHandler (sf::Vector2f mouse) ;

    void SetEnabled (bool b) { push_title.SetClickable(b) ; } ;

protected:
    void ResizeRect () ;

protected:
    std::vector<PushButton*> table_pushbuttons ;
    PushButton push_title ;
    bool bUnrolled ;
    sf::RectangleShape rect_background ;
};

/// ////////////////////
///
/// INPUT
///
/// ////////////////////

/*
 * La classe Input d�crit les �l�ments GUI r�cup�rant les textes/chiffres entr�s au clavier.
 * Classe abstraite.
 */

class Input
{
public:
    Input (sf::Vector2f text_position, sf::String n_default_text) ;

    virtual bool ClicHandler (sf::Vector2f mouse) ; // g�re le clique de la souris.
    bool KeyboardHandler (sf::Event event) ; // R�cup�re le charact�re entr� au clavier et modifie le texte si l'input a effectivement le focus.

    void Draw (sf::RenderWindow *window) ;
    bool HasFocus () const { return bUsed ; } ;

protected:
    virtual bool WriteInto (sf::Event event) = 0 ;

protected:
    bool bUsed ; // Pr�cise si l'input est en cours d'utilisation ou non.
    bool bInput_last_cycle ; // Indique si un caract�re a �t� entr� au cycle pr�c�dent.
    bool bEnabled ; // Indique si l'input est utilisable ou non.
    bool bUpdated ; // Indique si l'input a �t� modifi�.
    sf::Text text ; // Texte entr� par l'utilisateur au clavier.
    sf::String default_text ; // Texte affich� par d�faut lorsque l'input est vide.
    sf::Font font ; // Police d'�criture.
    sf::Color c_normal ; // Couleur du texte de l'input par d�faut.
    sf::Color c_used ;
};

/// ////////////////////
///
/// INPUT NUMBER
///
/// ////////////////////

/*
 * La classe InputNumber d�crit les �l�ments GUI r�cup�rant les chiffres entr�s au clavier.
 */

 class InputNumber : public Input
 {
public:
    InputNumber (sf::Vector2f text_position, sf::String default_value, int n_bound_down, int n_bound_up) ;
    virtual bool ClicHandler (sf::Vector2f mouse) ;
    void SetValue (int value) ;

    bool GetValueIfUpdated (int &new_value) ;

protected:
    virtual bool WriteInto (sf::Event event) ;
    void CheckInput () ;

protected:
    int bound_up ;
    int bound_down ;
 };

#endif // _GUI_H_
