#include "mbed.h"

/** Librairie Composant HX711
 * 
 * La classe HX711 est utilisée avec le composant du même nom
 *  Lien Datasheet : http://www.dfrobot.com/image/data/SEN0160/hx711_english.pdf
 *  Convertisseur Analogique/Numérique 24 bits
 *
 *  Mesure expérimentale personnelle : 
 *  Erreur relative d'environ 500 points sur 16777216 soit environ 0.00003% d'erreurs (9 bits inutiles)
 *  Utilisé avec un capteur de poids 0-30kg, précision a +/- 2g près
 *
 *  CORVASIER ADRIEN
 *  INRA - St Gilles
 */

class HX711
{
public:
    /** Constructeur de l'objet HX711
    *
    * @param pin_din : Pin assigné à la broche DT de la carte HX711
    * @param pin_slk : Pin assigné à la broche d'horloge SCK de la carte HX711
    * @returns aucun
    */
    HX711(PinName pin_din, PinName pin_slk);
    /** Destructeur de l'objet HX711
    *
    * @param numero : Numéro que l'on souhaite assigné
    * @returns numero
    */
    virtual ~HX711();
    /** Fonction permettant de déclencher la convertion et de récupérer la valeur mesurée entre 0 et 2^24
    *
    * @param aucun
    * @returns long Valeur mesurée sur un entier de type long
    */
    long getValue();
    /** Fonction permettant de déclencher la convertion et de récupérer la valeur mesurée entre -2^23 et 2^23
    *
    * @param aucun
    * @returns long Valeur mesurée sur un entier de type long
    */
    long getSignedValue();
    /** Fonction permettant de déclencher la convertion d'une série de mesure et ensuite de moyenner
    *
    * @param unsigned char Nombres de mesures à moyenner
    * @returns long Valeur mesurée et moyennée sur un entier de type long
    */
    long averageValue(unsigned char times = 32);
    /** Fonction permettant de régler l'offset pour la convertion en grammmes
    *
    * @param long Offset de décalage
    * @returns aucun
    */
    void setOffset(long offset);
    /** Fonction permettant de régler l'échelle pour la convertion en grammmes
    *
    * @param long Echelle de convertion
    * @returns aucun
    */
    void setScale(float scale);
    /** Fonction permettant de récupérer la mesure directement en grammes
    *
    * @param aucun
    * @returns int Grammes mesurés
    */
    int getGram();

private:                        // Déclaration des propriétés de l'objet
    DigitalIn _pin_dout;        // Broche de donnée
    DigitalOut _pin_slk;        // Broche d'horloge
    long _offset;               // Offset pour la convertion en grammes
    float _scale;               // Echelle pour la convertion en grammes
};

