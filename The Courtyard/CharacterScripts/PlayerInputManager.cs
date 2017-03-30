using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerInputManager : MonoBehaviour {

    [HideInInspector]
    public PlayerController mController;

	// Use this for initialization
	void Start () {
        mController = GetComponent<PlayerController>();
	}
	
	// Update is called once per frame
	void Update () {
        
        if (Input.GetButtonDown("Interact"))
            mController.PressInteract();
        else if (Input.GetButtonUp("Interact"))
            mController.ReleaseInteract();

        if (Input.GetButtonDown("Drop"))
            mController.PressDrop();
        else if (Input.GetButtonUp("Drop"))
            mController.ReleaseDrop();

        if (Input.GetButtonDown("CycleNext"))
            mController.PressCycleNext();
        else if (Input.GetButtonDown("CyclePrev"))
            mController.PressCyclePrev();
    }
}
