using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour {

    public PlayerCharacter mPlayerCharacter;

    [HideInInspector]
    public PlayerInputManager mInputManager;

	// Use this for initialization
	void Start () {
        mInputManager = GetComponent<PlayerInputManager>();

        if(mPlayerCharacter)
            mPlayerCharacter.mPlayerController = this;

    }

    public void Possess(PlayerCharacter inCharacter)
    {
        mPlayerCharacter = inCharacter;
        mPlayerCharacter.mPlayerController = this;
        // in the future if you want to play as another character you can put
        // code in here that moves the camera to the other character's camera
    }

    public void PressInteract()
    {
        mPlayerCharacter.PressInteract();
    }

    public void ReleaseInteract()
    {
        mPlayerCharacter.ReleaseInteract();
    }

    public void PressDrop()
    {
        mPlayerCharacter.PressDrop();
    }

    public void ReleaseDrop()
    {
        mPlayerCharacter.ReleaseDrop();
    }

    public void PressCycleNext()
    {
        mPlayerCharacter.PressCycleNext();
    }

    public void PressCyclePrev()
    {
        mPlayerCharacter.PressCyclePrev();
    }

    // Update is called once per frame
    void Update () {
		
	}
}
