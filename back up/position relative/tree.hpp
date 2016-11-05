#ifndef _TREE_H_
#define _TREE_H_

#include <vector>
#include <string>
#include "GUI.hpp"

#define PI 3.14159265358979323846

struct KeyFrame ;
class TreeItem ;

struct sprite_t
{
    unsigned int ID ;
    sf::Texture texture ;
    sf::Sprite sprite ;
    bool bSelected ;
    TreeItem *tree_item ;
    float rotation ;
};

/* La classe TreeItem est utilisée comme une structure. Donc fi de l'encapsulation. */
class TreeItem
{
public:
    TreeItem (std::string s, unsigned int ID) ;
    ~TreeItem () ;

    TreeItem *father ;
    std::vector<TreeItem*> children ;
    PushButton tree_button ;
    sprite_t m_sprite ;
    std::string name ;
};

TreeItem* BuildTree (std::vector<sprite_t *> *sprites_displaying) ;

/*
    Passer la racine de l'arbre en paramètre détruira totalement l'arbre.
*/
void BurnTree (TreeItem *item) ;

/*
    Cherche un élément de l'arbre par son nom.
    Si elle ne trouve pas, l'élément est crée.
 */
TreeItem* FindTreeItem(std::string name, TreeItem* item) ;
TreeItem* FindTreeItemById(unsigned int ID, TreeItem* item) ;
TreeItem* CreateTreeItem(std::string name, unsigned int &ID, std::vector<sprite_t *> *sprites_displaying) ;

/*
    Dessine l'arbre sur la fenêtre window avec comme origine le point (X,Y).
 */
void CalculateItemsPosition (TreeItem *root, int X, int Y) ;
void CalculateRecursive (TreeItem *item, int X, int *Y, unsigned int deepness) ;
void DrawTree (sf::RenderWindow * window, TreeItem *item) ;

void TreeClicHandler(TreeItem* root, const sf::Vector2f &mouse, std::vector<sprite_t *> *sprites_displaying) ;
TreeItem* IsItemClicked(TreeItem *item, const sf::Vector2f &mouse) ; // Utilisée par ClicHandler.
void UnselectPushButtons(TreeItem *item) ;
bool UnselectPushButton(unsigned int ID) ; // Utilisée par ClicHandler.
/*
    Retourne true si le PushButton a été trouvé, false sinon.
 */
void SelectPushButtonFromAnimationWindow (TreeItem *item, unsigned int ID) ;

void TranslateWithChildren (TreeItem *item, KeyFrame *key) ;
void TranslateWithChildren (TreeItem *item, sf::Vector2f translation) ;
void TranslateWithoutChildren (TreeItem *item, sf::Vector2f translation) ;
void RotateWithChildren (TreeItem *item, KeyFrame *key) ; /* Angle en degrés. */
void RotateWithChildren (TreeItem *item, sf::Vector2f gravity_center, const double angle, unsigned int deepness) ; /* Angle en degrés. */
void RotateWithouthChildren (TreeItem *item, double angle) ; /* Angle en degrés. */
void RotateVector (sf::Vector2f &to_rotate, sf::Vector2f &rotation_center, double angle) ; /* L'angle de rotation est en degrés. */
double NormeVector (sf::Vector2f v) ;

std::vector<std::string> SplitString(std::string s) ;

#endif // _TREE_H_
