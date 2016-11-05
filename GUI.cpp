#include "GUI.hpp"

/// DEBUGGING
#include <iostream>
#include <sstream>

/// ////////////////////
///
/// LABEL
///
/// ////////////////////

using namespace std ;

void Label::InitializeAttributes ()
{
    if (!font.loadFromFile("ressources/polices/Eligible.ttf"))
    {
        std::cout << "ERREUR: Impossible de charger la police Eligible.ttf" << std::endl ;
        system("pause") ;
        exit(-1) ;
    }

    c_normal = sf::Color::White ;

    bVisible = true ;
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


void Label::Draw (sf::RenderWindow *w)
{
    if (!bVisible) return ;

    if (hasTexture) w->draw(sprite) ;

    if (hasText) w->draw(text) ;
}

void Label::SetTexture (const sf::Texture &n_texture)
{
    sprite.setTexture(n_texture) ;

    hasTexture = true ;
}

/* Le comportement de SetPosition d�pend des bool�ens hasTexture et hasText */
void Label::SetPosition (int X, int Y)
{
    if (hasTexture)
    {
        // Le sprite est affich� avant le texte.
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

    hasText = true ;
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
    bHovered_last_cycle = false ;

    c_selected.r = 153 ;
    c_selected.g = 153 ;
    c_selected.b = 255 ;

    c_hovered.r = 28 ;
    c_hovered.g = 134 ;
    c_hovered.b = 238 ;
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


/*
   Le comportement de isClicked d�pend du bool�en bClickable.
   Si le bouton est d�sactiv�, il n'est pas cliquable.
 */
bool PushButton::isClicked (int X, int Y)
{
    return isClicked(sf::Vector2f(X,Y)) ;
}

/*
   Le comportement de isClicked d�pend du bool�en bClickable.
   Si le bouton est d�sactiv�, il n'est pas cliquable.
 */
bool PushButton::isClicked (sf::Vector2f m)
{
    if (!bClickable or !bVisible) return false ;

    bool bTexture = false, bText = false ;
    if (hasTexture)
        bTexture = sprite.getGlobalBounds().contains(m) ;

    if (hasText)
        bText = text.getGlobalBounds().contains(m) ;

    return (bText + bTexture) ;
}


void PushButton::HoverHandler (sf::Vector2f mouse)
{
    if (text.getGlobalBounds().contains(mouse) && !bHovered_last_cycle)
    {
        text.setColor(c_hovered) ;
        bHovered_last_cycle = true ;
    }
    else if (!text.getGlobalBounds().contains(mouse) && bHovered_last_cycle)
    {
        text.setColor(c_normal) ;
        bHovered_last_cycle = false ;
    }
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



/// ////////////////////
///
/// COMBO BOX
///
/// ////////////////////

ComboBox::ComboBox (sf::String text, int X, int Y) : push_title (text, X,Y)
{
    bUnrolled = false ;
    rect_background.setPosition(X-MARGIN, Y-MARGIN) ;
    rect_background.setSize(sf::Vector2f(2*MARGIN + push_title.GetText().getGlobalBounds().width, 2*MARGIN + push_title.GetText().getGlobalBounds().height)) ;
    rect_background.setFillColor(sf::Color(180,180,180)) ;
}

ComboBox::ComboBox (const sf::Texture &tex, sf::String text, int X, int Y) : push_title (text, tex, X, Y)
{
    ComboBox(text, X, Y) ;
}

ComboBox::~ComboBox()
{
    for (unsigned int i = 0 ; i < table_pushbuttons.size() ; i ++) delete table_pushbuttons[i] ;
}

void ComboBox::Draw(sf::RenderWindow *window)
{
    if (bUnrolled)
    {
        window->draw(rect_background) ;
        for (unsigned int i = 0 ; i < table_pushbuttons.size() ; i ++) table_pushbuttons[i]->Draw (window) ;
    }

    push_title.Draw(window) ;
}

void ComboBox::HoverHandler(sf::Vector2f mouse)
{
    push_title.HoverHandler(mouse) ;
    for (unsigned int i = 0 ; i < table_pushbuttons.size() ; i ++) table_pushbuttons[i]->HoverHandler(mouse) ;
}

void ComboBox::AddPushButton(sf::String text)
{
    PushButton *new_button = new PushButton (text,
                                            push_title.GetText().getPosition().x,
                                            push_title.GetText().getPosition().y + (MARGIN+push_title.GetText().getGlobalBounds().height)*(table_pushbuttons.size()+1)) ;

    table_pushbuttons.push_back(new_button) ;
    ResizeRect() ;
}

int ComboBox::ClicHandler(sf::Vector2f mouse)
{
    if (bUnrolled)
    {
        for (unsigned int i = 0 ; i < table_pushbuttons.size() ; i ++)
        {
            if (table_pushbuttons[i]->isClicked (mouse))
            {
                bUnrolled = false ;
                return (int)i ;
            }
        }

        bUnrolled = false ;
    }
    else
    {
        if (push_title.isClicked(mouse)) bUnrolled = true ;
    }

    return (-1) ;
}

void ComboBox::ResizeRect()
{
    float max_width = push_title.GetText().getGlobalBounds().width ;
    for (unsigned int i = 0 ; i < table_pushbuttons.size() ; i ++)
    {
        if (table_pushbuttons[i]->GetText().getGlobalBounds().width > max_width) max_width = table_pushbuttons[i]->GetText().getGlobalBounds().width ;
    }

    rect_background.setSize(sf::Vector2f(2*MARGIN+max_width , (table_pushbuttons.size()+3)*MARGIN + (table_pushbuttons.size()+1)*push_title.GetText().getGlobalBounds().height )) ;
}

/// ////////////////////
///
/// INPUT
///
/// ////////////////////

Input::Input (sf::Vector2f text_position, sf::String n_default_text)
{
    if (!font.loadFromFile("ressources/polices/Eligible.ttf"))
    {
        std::cout << "ERREUR: Impossible de charger la police Eligible.ttf" << std::endl ;
        system("pause") ;
        exit(-1) ;
    }

    c_normal = sf::Color::White ;

    c_used.r = 28 ;
    c_used.g = 134 ;
    c_used.b = 238 ;

    bUsed = false ;
    bInput_last_cycle = false ;
    bEnabled = true ;
    bUpdated = false ;

    default_text = n_default_text ;
    text.setFont(font) ;
    text.setCharacterSize(18) ;
    text.setPosition(text_position) ;
    text.setString(default_text) ;
    text.setColor(c_normal) ;
}

bool Input::ClicHandler(sf::Vector2f mouse)
{
    if (!bEnabled) return false ;

    bUsed = text.getGlobalBounds().contains(mouse) ;

    if (bUsed) text.setColor(c_used) ;
    else text.setColor(c_normal) ;

    return bUsed ;
}

bool Input::KeyboardHandler (sf::Event event)
{
    if (!bUsed or !bEnabled) return false ;
    return WriteInto (event) ;
}

void Input::Draw (sf::RenderWindow *window)
{
    window->draw(text) ;
}

/// ////////////////////
///
/// INPUT NUMBER
///
/// ////////////////////

InputNumber::InputNumber(sf::Vector2f text_position, sf::String default_value, int n_bound_down, int n_bound_up) : Input (text_position, default_value)
{
    bound_down = n_bound_down ;
    bound_up = n_bound_up ;
    text.setString (default_value) ;
}

bool InputNumber::ClicHandler (sf::Vector2f mouse)
{
    if (!bEnabled) return false ;

    bUsed = text.getGlobalBounds().contains(mouse) ;

    if (bUsed) text.setColor(c_used) ;
    else
    {
        text.setColor(c_normal) ;
        CheckInput() ;
    }

    return bUsed ;
}

void InputNumber::SetValue (int value)
{
    ostringstream oss ;
    oss << value ;
    text.setString(oss.str()) ;
    CheckInput() ;
}

bool InputNumber::WriteInto (sf::Event event)
{
    sf::String s = "" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0)) s = "0" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) s = "1" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) s = "2" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3)) s = "3" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) s = "4" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5)) s = "5" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)) s = "6" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad7)) s = "7" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8)) s = "8" ;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9) or sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9)) s = "9" ;

    if (!s.isEmpty() and !bInput_last_cycle)
    {
        text.setString(text.getString() + s) ;
        bInput_last_cycle = true ;
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace) and !bInput_last_cycle)
    {
        string new_string = text.getString() ;

        if (new_string.size() > 0)
        {
            new_string.resize(new_string.size()-1) ; // On redimensionne le texte en supprimant le dernier caract�re.
            text.setString(new_string) ;
        }
        else text.setString("") ;

        bInput_last_cycle = true ;
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) and !bInput_last_cycle)
    {
        bUsed = false ;
        text.setColor(c_normal) ;
        CheckInput() ;

        bInput_last_cycle = true ;
    }
    else bInput_last_cycle = false ;

    return bInput_last_cycle ;
}

bool InputNumber::GetValueIfUpdated (int &new_value)
{
    if (!bUpdated) return false ;

    bUpdated = false ;
    string number = text.getString() ;
    new_value = atoi (number.c_str()) ;
    return true ;
}

void InputNumber::CheckInput()
{
    string s = text.getString() ;

    if (s.empty()) // Si la chaine est vide, on affiche le texte par d�faut.
    {
        text.setString(default_text) ;
        return ;
    }

    int number = atoi (s.c_str()) ; // On convertit la chaine de caract�res en nombre.
    ostringstream oss ;
    if (number < bound_down)
    {
        oss << bound_down ;
        text.setString(oss.str()) ;
    }
    else if (number > bound_up)
    {
        oss << bound_up ;
        text.setString(oss.str()) ;
    }

    bUpdated = true ;
}
