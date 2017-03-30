using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerInventoryManager : MonoBehaviour {

    [HideInInspector]
    public PlayerCharacter mPlayerCharacter;
    private PickupableObject[] mInventory;
    public int maxInventorySize = 4;

    // if you need to lock the player's inputs on the inventory for whatever reason
    public bool bLocked = false;

    private Vector3 startPos;
    public float heavyInterpSpeeed = 1.0f;
    private float totalDistance;
    private float startTime;
    private PickupableObject tempHeavyObj;

    private int tempIndex = 0;
    private int currentIndex = 0;
    private int itemCount = 0;

    bool bShouldInterpolateHeavy = false;

    public GameObject rightHandSmallSocket;
    public GameObject rightHandLargeSocket;

    // Use this for initialization
    void Start () {
        mInventory = new PickupableObject[maxInventorySize];
        mPlayerCharacter = GetComponent<PlayerCharacter>();
    }

    public PickupableObject GetCurrentObject()
    {
        return mInventory[currentIndex];
    }

    // Parent the object to the hand socket
    // TODO: maybe move this to the object's code instead
    public void EquipObject(PickupableObject inObj)
    {
        if (bLocked)
            return;

        // look for the first available space
        // TODO: rearrange objects after pickup
        int index = -1;
        for(int i = 0; i < maxInventorySize; i++)
        {
            if(mInventory[i] == null)
            {
                index = i;
                break;
            }
        }

        if(index == -1)
        {
            Drop();
            EquipObject(inObj);
            return;
        }
        // there is a space available at index
        if(index != -1)
        {

            // unshow the current weapon
            if (GetCurrentObject())
                GetCurrentObject().gameObject.SetActive(false);

            // disable collision for current weapon


            mInventory[index] = inObj;
            itemCount++;

            mInventory[index].gameObject.layer = LayerMask.NameToLayer("SmallInteractables");

            tempIndex = index;

            mPlayerCharacter.mInventoryManager.bLocked = true;

            if (inObj.mWeightClass == PickupableObject.PickupWeightClass.Light)
                mPlayerCharacter.mAnimationManager.PlayPickupLightAnimation();
            else if (inObj.mWeightClass == PickupableObject.PickupWeightClass.Heavy)
                mPlayerCharacter.mAnimationManager.PlayPickupHeavyAnimation();

        }
        else
        {
            // drop current object for new weapon
            
        }
    }

    // todo: move this to object code
    public void EquipLightEffects()
    {
        mInventory[tempIndex].transform.parent = rightHandSmallSocket.transform;
        ToggleObjectRigidbodyKinematics(mInventory[tempIndex], true);
    }

    public void EquipHeavyEffects()
    {
        //mInventory[tempIndex].transform.parent = rightHandLargeSocket.transform;
        ToggleObjectRigidbodyKinematics(mInventory[tempIndex], true);

        tempHeavyObj = mInventory[tempIndex];

        startPos = tempHeavyObj.transform.position;
        startTime = Time.time;
        bShouldInterpolateHeavy = true;
        // todo: interpolate to the socket instead of snapping to it
    }

    public void FinishedPickingUp()
    {
        bLocked = false;
        mInventory[tempIndex].gameObject.SetActive(false);

        if(mInventory[tempIndex].mWeightClass == PickupableObject.PickupWeightClass.Light)
            mInventory[tempIndex].transform.position = rightHandSmallSocket.transform.position;
        else
        {
            mInventory[tempIndex].transform.parent = rightHandLargeSocket.transform;
            mInventory[tempIndex].transform.position = rightHandLargeSocket.transform.position;
            bShouldInterpolateHeavy = false;
        }


        SwapToObject(tempIndex);
    }

    public void SwapToObject(int index)
    {
        if (bLocked)
            return;

        // TODO: move this to the actual object and add animations effects etc.
        // deactivate current object
        if (GetCurrentObject())
            GetCurrentObject().gameObject.SetActive(false);

        currentIndex = index;

        // activate new object
        mInventory[index].gameObject.SetActive(true);

        // disable shadows
        mInventory[index].GetComponent<MeshRenderer>().shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;

        if (GetCurrentObject())
            mPlayerCharacter.mAnimationManager.SetHoldingWeapon(true);
        else
            mPlayerCharacter.mAnimationManager.SetHoldingWeapon(false);

        mPlayerCharacter.mAnimationManager.SwapWeapon();
    }

    // need to fix this shit
    public void CycleNext()
    {
        if (bLocked)
            return;
       
        for (int i = 0; i < maxInventorySize; i++)
        {
            int temp = (i + currentIndex) % maxInventorySize;

            if(mInventory[temp])
            {
                SwapToObject(temp);
            }
        }
    }

    public void CyclePrev()
    {
        if (bLocked)
            return;

        for (int i = maxInventorySize; i >= 0; i--)
        {
            int temp = (i + currentIndex) % maxInventorySize;

            if (mInventory[temp])
            {
                SwapToObject(temp);
            }
        }
    }

    void ToggleObjectRigidbodyKinematics(PickupableObject obj, bool b)
    {
        Rigidbody[] bodies = obj.GetComponentsInChildren<Rigidbody>();
        foreach (Rigidbody r in bodies)
        {
            r.isKinematic = b;
            r.detectCollisions = !b;
        }
    }

    public void DropObject(int index)
    {
        if (bLocked)
            return;

        if (!GetCurrentObject())
            return;

        mInventory[index].transform.parent = null;
        ToggleObjectRigidbodyKinematics(mInventory[index], false);
        mInventory[index].gameObject.layer = mInventory[index].gameObject.layer = LayerMask.NameToLayer(mInventory[index].layerName);



        // enable shadows
        mInventory[index].GetComponent<MeshRenderer>().shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.On;

        if (mInventory[index].mWeightClass == PickupableObject.PickupWeightClass.Heavy)
            mPlayerCharacter.mAnimationManager.PlayDropHeavy();
         
        mInventory[index] = null;
        itemCount--;



        // find next available object
        CycleNext();

        if (GetCurrentObject())
            mPlayerCharacter.mAnimationManager.SetHoldingWeapon(true);
        else
            mPlayerCharacter.mAnimationManager.SetHoldingWeapon(false);

    }

    public void Drop()
    {
        DropObject(currentIndex);
    }


    // TODO: place object down at a position
    public void Place()
    {
        DropObject(currentIndex);
    }

    // Update is called once per frame
    void Update () {
		if(bShouldInterpolateHeavy)
        {
            float currentDistance = (Time.time - startTime) * heavyInterpSpeeed;
            totalDistance = Vector3.Distance(startPos, rightHandLargeSocket.transform.position);
            float traveledDistance = currentDistance / totalDistance;
            tempHeavyObj.transform.position = Vector3.Lerp(startPos, rightHandLargeSocket.transform.position, traveledDistance);
        }
	}
}
