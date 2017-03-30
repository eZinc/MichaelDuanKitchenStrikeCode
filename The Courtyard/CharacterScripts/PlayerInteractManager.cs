using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerInteractManager : MonoBehaviour {
   
    [HideInInspector]
    public InteractableObject mCurrentObject;

    [HideInInspector]
    public PlayerCharacter mPlayerCharacter;

	// Use this for initialization
	void Start () {
        mPlayerCharacter = GetComponent<PlayerCharacter>();
    }

    public void Interact()
    {
        if (mCurrentObject)
            mCurrentObject.OnInteract(mPlayerCharacter);
        else
            mPlayerCharacter.mInventoryManager.GetCurrentObject().OnInteract(mPlayerCharacter);
    }

    public void OffInteract()
    {
        if(mCurrentObject)
            mCurrentObject.OffInteract(mPlayerCharacter);
        else
            mPlayerCharacter.mInventoryManager.GetCurrentObject().OffInteract(mPlayerCharacter);

    }

    public bool RaycastForInteractables()
    {
        // lazily get the main camera since we most likely will not have out of body experiences
        Camera mCamera = Camera.main;
        if (mCamera)
        {
            Vector3 mPos = mCamera.transform.position;
            Vector3 direction = mCamera.transform.forward;

            RaycastHit hitInfo;

            if (Physics.Raycast(mPos, direction, out hitInfo))
            {
                // check if it hits an interactable object
                mCurrentObject = hitInfo.transform.GetComponent<InteractableObject>();
                if (mCurrentObject)
                {
                    // todo: use squared distance
                    float distanceToObject = (mCurrentObject.transform.position - mCamera.transform.position).sqrMagnitude;
                    // check how far away it is and if it is eligible to be interacted with
                    if (mCurrentObject.maxInteractableDistance * mCurrentObject.maxInteractableDistance >= distanceToObject)
                    {
                        return true;
                    }
                    else
                    {
                        mCurrentObject = null;
                    }
                }
            }
        }

        // tell ui that no interactable was hit
        return false;
    }
	
	// Update is called once per frame
	void Update () {

        // check what is in front if it every frame by raycast
        if(RaycastForInteractables())
        {
            mPlayerCharacter.mUIManager.HoveredInteractable(mCurrentObject);
        }
        else
        {
            mPlayerCharacter.mUIManager.UnhoveredInteractable();
        }

    }
}
