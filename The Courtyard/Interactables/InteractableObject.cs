using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InteractableObject : MonoBehaviour {

    public string mName = "ObjectName";
    public string mDescriptionText = "Press F to examine";
    public float maxInteractableDistance = 50.0f;

	// Use this for initialization
	void Start () {
		
	}

    public virtual void OnInteract(PlayerCharacter inChar)
    {

    }

    public void OffInteract(PlayerCharacter inChar)
    {

    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
