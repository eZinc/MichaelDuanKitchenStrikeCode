using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerAnimationManager : MonoBehaviour {

    public Animator Animator;

    public PlayerCharacter mPlayerCharacter;

    // Use this for initialization
    void Start () {
		
	}

    public void SetHoldingWeapon(bool b)
    {
        Animator.SetBool("IsHoldingWeapon", b);
    }

    public void SwapWeapon()
    {
        Animator.SetTrigger("SwapWeapon");
    }

    public void PlayDropHeavy()
    {
        Animator.SetTrigger("DropHeavy");
    }

    public void PlayPickupLightAnimation()
    {
        Animator.SetTrigger("PickupLight");
    }

    public void PlayPickupHeavyAnimation()
    {
        Animator.SetTrigger("PickupHeavy");
    }

    public void PickupHeavyObject()
    {
        mPlayerCharacter.mInventoryManager.EquipHeavyEffects();
    }

    // this is called when the hand smacks on object
    public void PickupLightObject()
    {
        mPlayerCharacter.mInventoryManager.EquipLightEffects();
    }

    // called when picking up animation is done
    public void DonePickingUp()
    {
        mPlayerCharacter.mInventoryManager.FinishedPickingUp();
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
