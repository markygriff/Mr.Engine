/* Custom ContactListener class for Box2D
    - if the contact is with a player, we make the other fixture get punched if the player is punching 
*/

#ifndef ContactListener_hpp
#define ContactListener_hpp

#include <Box2D/Box2D.h>
#include <stdio.h>

class ContactListener : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    
private:
    
};

#endif /* ContactListener_hpp */
