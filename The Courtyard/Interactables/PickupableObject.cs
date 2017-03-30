using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PickupableObject : InteractableObject {

    public enum PickupWeightClass { Light, Heavy };
    public PickupWeightClass mWeightClass;
    public float DistancePlacedFromPlayer = 0.0f;
    public Transform equipSocket;

    [HideInInspector]
    public string layerName;

	// Use this for initialization
	void Start () {
        layerName = LayerMask.LayerToName(gameObject.layer);

    }

    // pick up the object
    public override void OnInteract(PlayerCharacter inChar)
    {
        // if it's not in the inventory, equip it
        if (inChar.mInventoryManager.GetCurrentObject() != this)
            inChar.mInventoryManager.EquipObject(this);
        // else, place or drop it depending on light or heavy
        else if (mWeightClass == PickupWeightClass.Light)
            inChar.mInventoryManager.Place();
        else if (mWeightClass == PickupWeightClass.Heavy)
            inChar.mInventoryManager.Drop();
    }

    // Update is called once per frame
    void Update () {
		
	}
}
