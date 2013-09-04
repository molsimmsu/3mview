#ifndef VRN_MOLECULECOLLECTION_H
#define VRN_MOLECULECOLLECTION_H

#include "voreen/core/utils/observer.h"
#include "molecule.h"

#include <vector>
#include <string>

namespace voreen {

class MoleculeCollection;
class Modality;

/**
 * Interface for molecule collection observers.
 */
class VRN_CORE_API MoleculeCollectionObserver : public Observer {
public:

    /**
     * This method is called by the observed collection after
     * a molecule handle has been added.
     *
     * @param source the calling collection
     * @param handle the molecule handle that has been added
     */
    virtual void moleculeAdded(const MoleculeCollection* /*source*/, const Molecule* /*handle*/) {};

    /**
     * This method is called by the observed collection after
     * a molecule handle has been removed.
     *
     * @param source the calling collection
     * @param handle the molecule handle that has been removed
     */
    virtual void moleculeRemoved(const MoleculeCollection* /*source*/, const Molecule* /*handle*/) {};

    /**
    * This method is called by the observed collection after
    * a molecule handle has been changed. This usually happens when
    * a molecule handle reloads the wrapped molecule (i.e. underlying structure has changed).
    *
    * @param source the calling collection
    * @param handle the molecule handle that has been changed
    */
    virtual void moleculeChanged(const MoleculeCollection* /*source*/, const Molecule* /*handle*/) {};
    
    /**
    * This method is called by the observed collection after
    * a molecule has been transformed. This happens when
    * a setTransformationMatrix() is called over a given molecule.
    *
    * @param source the calling collection
    * @param handle the molecule handle that has been changed
    */
    virtual void moleculeTransformed(const MoleculeCollection* /*source*/, const Molecule* /*handle*/, const tgt::mat4& matrix) {};

};

#ifdef DLL_TEMPLATE_INST
template class VRN_CORE_API Observable<MoleculeCollectionObserver>;
#endif

/**
 * Collection of molecule handles that can be observed.
 *
 * The collection does not perform memory management, i.e., added molecule handles
 * are not deleted on removal or destruction of the collection.
 *
 * @see MoleculeContainer
 */
class VRN_CORE_API MoleculeCollection : public Observable<MoleculeCollectionObserver>, protected MoleculeObserver {

public:

    MoleculeCollection();

    virtual ~MoleculeCollection();

    /**
     * Adds the passed Molecule to the collection,
     * if it is not already contained.
     *
     * The Collection does not take ownership of the
     * added Molecule and does therefore not delete it
     * on its own destruction.
     */
    virtual void add(Molecule* moleculeHandle);

    /**
     * Adds MoleculeHandles contained by the passed MoleculeCollection
     * to the collection, if they are not already contained.
     *
     * The Collection does not take ownership of the
     * added MoleculeHandles and does therefore not delete them
     * on its own destruction.
     */
    virtual void add(const MoleculeCollection* moleculeCollection);

    /**
     * Removes the passed Molecule from the Collection
     * without deleting it.
     */
    virtual void remove(const Molecule* moleculeHandle);

    /**
     * Removes all MoleculeHandles contained by the passed collection
     * without deleting them.
     */
    virtual void remove(const MoleculeCollection* moleculeCollection);

    /**
     * Returns whether the passed Molecule is contained
     * by the collection.
     */
    virtual bool contains(const Molecule* moleculeHandle) const;

    /**
     * Returns the MoleculeElement at a specified index position.
     *
     * @param The index of the Molecule to return. Must be valid, i.e. i < size().
     */
    virtual Molecule* at(size_t i) const;

    /**
     * Returns the first Molecule of the collection, or null
     * if the collection is empty.
     */
    virtual Molecule* first() const;

    /**
     * Clears the collection without deleting the MoleculeHandles.
     */
    virtual void clear();
    
    /**
     * Returns a collection containing all Molecules with the
     * specified origin.
     */
    virtual MoleculeCollection* selectOrigin(const VolumeURL& origin) const;

    /**
     * Returns a new collection containing the molecules between the passed start and end indices (inclusive).
     */
    virtual MoleculeCollection* subCollection(size_t start, size_t end) const;

    /**
     * Returns a new collection containing the molecules at the passed indices.
     */
    virtual MoleculeCollection* subCollection(const std::vector<size_t>& indices) const;

    /**
     * Returns the number of MoleculeHandles contained by the collection.
     */
    virtual size_t size() const;

    /**
     * Returns whether the collection is currently empty.
     */
    virtual bool empty() const;

    /// @see MoleculeObserver::moleculeChange
    virtual void moleculeChange(const Molecule* handle);

    /// @see MoleculeObserver::moleculeDelete
    virtual void moleculeDelete(const Molecule* handle);
    
    /// @see MoleculeObserver::moleculeDelete
    virtual void moleculeTransform(const Molecule* handle, const tgt::mat4& matrix);

protected:
    /**
     * Returns an iterator pointing to the position of the passed Molecule
     * within the moleculeHandles_ vector. Returns moleculeHandles_.end(), if
     * the handle is not contained by the collection.
     */
    std::vector<Molecule*>::iterator find(const Molecule* moleculeHandle);
    std::vector<Molecule*>::const_iterator find(const Molecule* moleculeHandle) const;

    /// Notifies all MoleculeCollectionObservers that a handle has been added.
    void notifyMoleculeAdded(const Molecule* handle);
    /// Notifies all MoleculeCollectionObservers that a handle has been removed.
    void notifyMoleculeRemoved(const Molecule* handle);
    /// Notifies all MoleculeCollectionObservers that a handle has been changed.
    void notifyMoleculeChanged(const Molecule* handle);
    /// Notifies all MoleculeCollectionObservers that a handle has been transformed.
    void notifyMoleculeTransformed(const Molecule* handle, const tgt::mat4& matrix);

    /// Vector storing the MoleculeHandles contained by the collection.
    std::vector<Molecule*> moleculeHandles_;

    /// category for logging.
    static const std::string loggerCat_;
};

}   // namespace

#endif
