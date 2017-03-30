using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LadderCollider : MonoBehaviour {

    public LadderObject mMasterLadder;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    void OnTriggerEnter(Collider collision)
    {
        // see if player is colliding, and if so tell the player that it is now on a ladder
        CustomFPSController fps = collision.gameObject.GetComponent<CustomFPSController>();
        if(fps)
        {
            fps.mCurrentLadder = mMasterLadder;
        }
    }

    void OnTriggerExit(Collider collision)
    {
        // see if player leaves, tell it that it's no longer on a ladder
        CustomFPSController fps = collision.gameObject.GetComponent<CustomFPSController>();
        if (fps)
        {
            if (fps.mCurrentLadder == mMasterLadder)
                fps.mCurrentLadder = null;
        }
    }
}
