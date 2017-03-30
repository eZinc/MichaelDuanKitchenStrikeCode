using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

public class PlayerUIManager : MonoBehaviour {

    public Text ObjectNameText;

    public Image CrosshairImage;
    public Sprite HandSprite;
    public float HandScale;
    public Sprite InspectSprite;
    public float InspectScale;
    public Sprite DefaultCrosshairSprite;
    public float DefaultCrosshairScale;

	// Use this for initialization
	void Start () {
        Cursor.visible = false;
        Cursor.lockState = CursorLockMode.Locked;
	}
	
    // todo: make it so it isn't called every frame maybe
    public void HoveredInteractable(InteractableObject inObj)
    {
        ObjectNameText.text = inObj.mName;

        if(inObj is PickupableObject)
        {
            // change to hand icon
            CrosshairImage.sprite = HandSprite;
            CrosshairImage.rectTransform.localScale = new Vector3(HandScale, HandScale, HandScale);
        }
    }

    // todo: make it so it isn't called every frame maybe
    public void UnhoveredInteractable()
    {
        ObjectNameText.text = "";
        CrosshairImage.sprite = DefaultCrosshairSprite;
        CrosshairImage.rectTransform.localScale = new Vector3(DefaultCrosshairScale, DefaultCrosshairScale, DefaultCrosshairScale);

    }

    // Update is called once per frame
    void Update () {
		
	}
}
