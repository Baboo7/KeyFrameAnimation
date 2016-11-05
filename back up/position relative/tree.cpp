#include "tree.hpp"
#include "sprite_manager.hpp"
#include "time_line.hpp"
#include <iostream>
#include <fstream>
#include <math.h>

#define MARGIN_TREE_X 20
#define MARGIN_TREE_Y 22
using namespace std ;

TreeItem::TreeItem(string s, unsigned int ID) : tree_button(s, 0, 0)
{
    name = s ;

    m_sprite.ID = ID ;
    if (!m_sprite.texture.loadFromFile("animation/" + name))
        cout << "ERREUR: La texture " << name << " n'a pas pu être chargée." << endl ;
    else
        m_sprite.sprite.setTexture(m_sprite.texture) ;

    m_sprite.bSelected = false ;
    m_sprite.tree_item = this ;
    m_sprite.rotation = 0.0f ;
}

TreeItem::~TreeItem()
{

}

TreeItem* BuildTree (vector<sprite_t *> *sprites_displaying)
{
    // Ouverture du fichier.
    string name_file = "animation/tree.txt" ;
    ifstream file (name_file.c_str()) ;

    if (!file)
    {
        cout << "ERREUR: Le fichier " << name_file.c_str() << " n'a pas été ouvert correctement." << endl ;
        system("pause") ;
        exit(-1) ;
    }

    // Lecture du fichier.
    string line ;
    string s_father ;
    string s_child ;
    vector<string> v_split ;

    if (!getline(file, line))
    {
        cout << "ERREUR: Le fichier animation.txt est vide." << endl ;
        system("pause") ;
        exit(-1) ;
    }

    // Création de la racine de l'arbre.
    unsigned int ID = 0 ;
    TreeItem *root = CreateTreeItem(line, ID, sprites_displaying) ;
    root->father = NULL ;

    // Construction de l'arbre.
    while (getline(file, line))
    {
        v_split = SplitString(line) ;
        s_father = v_split[0] ;
        s_child = v_split[1] ;

        // On recherche les items associés à ces noms.
        TreeItem *item_father = FindTreeItem(s_father, root) ;
        if (item_father == NULL) // Si l'item n'est pas trouvé, on le crée.
            item_father = CreateTreeItem(s_father, ID, sprites_displaying) ;

        TreeItem *item_child = FindTreeItem(s_child, root) ;
        if (item_child == NULL)
            item_child = CreateTreeItem(s_child, ID, sprites_displaying) ;

        item_father->children.push_back(item_child);
        item_child->father = item_father ;
    }

    file.close() ;

    root->tree_button.SetSelected(true) ;

    SelectSprite(&root->m_sprite) ;
    return root ;
}

void BurnTree (TreeItem *item)
{
    // On détruit les enfants.
    for(unsigned int i=0 ; i<item->children.size() ; i++)
        BurnTree(item->children[i]) ;

    // On ken le père.
    delete item ;
}

TreeItem* FindTreeItem(string name, TreeItem* item)
{
    if (item->name.compare(name) == 0)
        return item ;

    TreeItem *children_result = NULL ;
    for (unsigned int i=0 ; i<item->children.size() ; i++)
    {
        children_result = FindTreeItem(name, item->children[i]) ;
        if (children_result != NULL)
            return children_result ;
    }

    return NULL ;
}

TreeItem* FindTreeItemById(unsigned int ID, TreeItem* item)
{
    if (item->m_sprite.ID == ID)
        return item ;

    TreeItem *children_result = NULL ;
    for (unsigned int i=0 ; i<item->children.size() ; i++)
    {
        children_result = FindTreeItemById(ID, item->children[i]) ;
        if (children_result != NULL)
            return children_result ;
    }

    return NULL ;
}

TreeItem* CreateTreeItem (string name, unsigned int &ID, vector<sprite_t *> *sprites_displaying)
{
    TreeItem *item = new TreeItem(name, ID) ;
    ID ++ ;
    item->m_sprite.sprite.setColor(sf::Color(255,255,255,180)) ;
    sprites_displaying->push_back(&item->m_sprite) ;
    return item ;
}

void CalculateItemsPosition (TreeItem *root, int X, int Y)
{
    int common_Y = Y ;
    CalculateRecursive(root, X, &common_Y, 0) ;
}

void CalculateRecursive (TreeItem *item, int X, int *Y, unsigned int deepness)
{
    item->tree_button.SetPosition(X + deepness * MARGIN_TREE_X, *Y) ;
    *Y += MARGIN_TREE_Y ;

    for (unsigned int i=0 ; i<item->children.size() ; i++)
        CalculateRecursive(item->children[i], X, Y, deepness + 1) ;
}

void DrawTree (sf::RenderWindow * window, TreeItem *item)
{
    item->tree_button.Draw(window) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++)
        DrawTree(window, item->children[i]) ;
}

void TreeClicHandler(TreeItem* root, const sf::Vector2f &mouse, vector<sprite_t *> *sprites_displaying)
{
    TreeItem *item_clicked = IsItemClicked(root, mouse) ;
    if (item_clicked == NULL) // Si aucun bouton n'est cliqué, on désélectionne tous les PushButtons et tous les sprites.
    {
        UnselectPushButtons(root) ;

        // Mise à jour des sprites.
        UnselectSprites(sprites_displaying) ;
        return ;
    }

    if (item_clicked->tree_button.GetSelected()) // Si le bouton cliqué est déjà sélectionné, on ne fait rien.
        return ;

    UnselectPushButtons(root) ;
    item_clicked->tree_button.SetSelected(true) ;

    // Mise à jour des sprites.
    SelectSpriteFromTree(item_clicked->m_sprite.ID, sprites_displaying) ;
}

TreeItem* IsItemClicked(TreeItem *item, const sf::Vector2f &mouse)
{
    if (item->tree_button.isClicked(mouse))
        return item ;

    TreeItem *children_result = NULL ;
    for (unsigned int i=0 ; i<item->children.size() ; i++)
    {
        children_result = IsItemClicked(item->children[i], mouse) ;
        if (children_result != NULL)
            return children_result ;
    }

    return NULL ;
}

void UnselectPushButtons(TreeItem *item)
{
    item->tree_button.SetSelected(false) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++)
        UnselectPushButtons(item->children[i]);
}

void SelectPushButtonFromAnimationWindow (TreeItem *item, unsigned int ID)
{
    if (item->m_sprite.ID == ID && !item->tree_button.GetSelected())
        item->tree_button.SetSelected(true) ;
    else if (item->m_sprite.ID != ID && item->tree_button.GetSelected())
        item->tree_button.SetSelected(false) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++)
        SelectPushButtonFromAnimationWindow(item->children[i], ID) ;
}


void TranslateWithChildren (TreeItem *item, KeyFrame *key)
{
    TranslateWithChildren (item, key->table_positions[item->m_sprite.ID].position) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++) TranslateWithChildren (item->children[i], key) ;
}


void TranslateWithChildren (TreeItem *item, sf::Vector2f translation)
{
    item->m_sprite.sprite.move(translation) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++)
    {
        TranslateWithChildren (item->children[i], translation) ;
    }
}

void TranslateWithoutChildren (TreeItem *item, sf::Vector2f translation)
{
    item->m_sprite.sprite.move(translation) ;
}

void RotateWithChildren (TreeItem *item, KeyFrame *key)
{
    RotateWithChildren (item, item->m_sprite.sprite.getPosition(), key->table_positions[item->m_sprite.ID].rotation, 0) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++) RotateWithChildren (item->children[i], key) ;
}

void RotateWithChildren (TreeItem *item, sf::Vector2f rotation_center, const double angle, unsigned int deepness)
{
    if (deepness == 0) item->m_sprite.rotation += angle ;

    sf::Vector2f original_g_c = item->m_sprite.sprite.getPosition() ;

    RotateVector(original_g_c, rotation_center, angle) ;
    item->m_sprite.sprite.rotate((float) angle) ;
    item->m_sprite.sprite.setPosition(original_g_c) ;

    for (unsigned int i=0 ; i<item->children.size() ; i++) RotateWithChildren(item->children[i], rotation_center, angle, deepness+1) ;
}

void RotateWithouthChildren (TreeItem *item, double angle)
{
    item->m_sprite.sprite.rotate((float) angle) ;
    item->m_sprite.rotation += angle ;
}

void RotateVector (sf::Vector2f &to_rotate, sf::Vector2f &rotation_center, double angle)
{
    sf::Vector2f v = to_rotate - rotation_center ;

    double initial_norm = NormeVector(v) ;

    double  t = angle*PI/180.f ;
    v.x = v.x*cos(t) - v.y*sin(t) ;
    v.y = v.x*sin(t) + v.y*cos(t) ;
    double final_norm = NormeVector(v) ;

    if (final_norm != 0) // On redonne au vecteur sa taille initiale (erreur de calcul dûe aux approximations numériques).
    {
        v.x *= initial_norm/final_norm ;
        v.y *= initial_norm/final_norm ;
    }

    to_rotate = v + rotation_center ;
}

double NormeVector (sf::Vector2f v)
{
    return sqrt(powf(v.x, 2.0) + powf(v.y, 2.0)) ;
}

/*
    Découpe une string en deux à la rencontre du premier charactère '>'.
    Fonctionne.
 */
vector<string> SplitString(string s)
{
    vector<string> v ;
    const char *tab = s.c_str() ;
    unsigned int i = 0;
    while (tab[i] != '>')
    {
        i++ ;
        if (i>=s.size())
        {
            v.push_back(s) ;
            return v ;
        }
    }

    v.push_back(s.substr(0, i)) ;
    v.push_back(s.substr(i+1)) ;
    return v ;
}






















