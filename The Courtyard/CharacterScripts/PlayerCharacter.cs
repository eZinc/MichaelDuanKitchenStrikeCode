using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerCharacter : MonoBehaviour {

    [HideInInspector]
    public PlayerInventoryManager mInventoryManager;

    [HideInInspector]
    public PlayerUIManager mUIManager;

    [HideInInspector]
    public PlayerInteractManager mInteractManager;
    
    public PlayerAnimationManager mAnimationManager;

    public PlayerController mPlayerController;

    // Use this for initialization
    // Find all other relevant components of the character
    // Designers: you need to have these components in the same GameObject as the character
    void Start () {
        mInventoryManager = GetComponent<PlayerInventoryManager>();
        mUIManager = GetComponent<PlayerUIManager>();
        mInteractManager = GetComponent<PlayerInteractManager>();
    }

    public void PressInteract()
    {
        mInteractManager.Interact();
    }

    public void ReleaseInteract()
    {
        mInteractManager.OffInteract();
    }


    public void PressDrop()
    {
        if (mInventoryManager)
            mInventoryManager.Drop();
    }

    // maybe hold to throw object farther, does nothing for now
    public void ReleaseDrop()
    {
    }

    public void PressCycleNext()
    {
        if (mInventoryManager)
            mInventoryManager.CycleNext();
    }
    
    public void PressCyclePrev()
    {
        if (mInventoryManager)
            mInventoryManager.CyclePrev();
    }

    // Update is called once per frame
    void Update () {
		
	}
}
