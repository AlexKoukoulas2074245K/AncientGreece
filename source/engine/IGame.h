///------------------------------------------------------------------------------------------------
///  IGame.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef IGame_h
#define IGame_h

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------
/// The interface that lays out the core methods for the Game. This should be subclassed 
/// and supplied to the Engine's RunGame method \see GenesisEngine::RunGame()
class IGame
{
public:    
    virtual ~IGame() = default;

    /// Game systems initialization method. 
    ///
    /// This is the first initialization method to be called, and is 
    /// where all ecs systems used by the game should be initialized.
    /// The initialization order of the systems will also determine their order of execution.
    virtual void VOnSystemsInit() = 0;

    /// Game initialization method. 
    ///
    /// This will be called after all systems have been initialized, and 
    /// the game is now free to perform all its game-specific initialization flows.    
    virtual void VOnGameInit() = 0;

    /// Game update method. 
    ///
    /// This will be called once per frame and should be used to 
    /// perform all game-specific logic.
    /// @param[in] dt the inter-frame delta time.    
    virtual void VOnUpdate(const float dt) = 0;
};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* IGame_h */
