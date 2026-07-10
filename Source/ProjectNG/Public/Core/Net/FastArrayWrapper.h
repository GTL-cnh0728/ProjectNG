#pragma once

#define DECLARE_FAST_ARRAY_FUNCTIONS(ListType, EntryType, ItemType)          \
    void Add(ItemType* Item)                                                 \
    {                                                                        \
        EntryType& Entry = Items.AddDefaulted_GetRef();                      \
        Entry.Item = Item;                                                   \
        MarkItemDirty(Entry);                                                \
    }                                                                        \
                                                                             \
    bool Remove(ItemType* Item)                                              \
    {                                                                        \
        for (int32 i = 0; i < Items.Num(); ++i)                              \
        {                                                                    \
            if (Items[i].Item == Item)                                       \
            {                                                                \
                Items.RemoveAt(i);                                           \
                MarkArrayDirty();                                            \
                return true;                                                 \
            }                                                                \
        }                                                                    \
        return false;                                                        \
    }                                                                        \
                                                                             \
    bool Contains(ItemType* Item) const                                      \
    {                                                                        \
        for (const EntryType& Entry : Items)                                 \
        {                                                                    \
            if (Entry.Item == Item)                                          \
            {                                                                \
                return true;                                                 \
            }                                                                \
        }                                                                    \
        return false;                                                        \
    }                                                                        \
                                                                             \
    template<typename T>                                                     \
    void Append(const TArray<T*>& NewItems)                           \
    {                                                                        \
        for (T* Item : NewItems)                                      \
        {                                                                    \
            Add(Item);                                                       \
        }                                                                    \
    }                                                                        \
                                                                             \
    template<typename T>                                                     \
    void Append(const TArray<TObjectPtr<T>>& NewItems)                \
    {                                                                        \
        for (T* Item : NewItems)                                      \
        {                                                                    \
            Add(Item);                                                       \
        }                                                                    \
    }                                                                        \
                                                                             \
    int32 Num() const                                                        \
    {                                                                        \
        return Items.Num();                                                  \
    }                                                                        \
                                                                             \
    bool IsEmpty() const                                                     \
    {                                                                        \
        return Items.IsEmpty();                                              \
    }                                                                        \
                                                                             \
    EntryType& operator[](int32 Index)                                       \
    {                                                                        \
        return Items[Index];                                                 \
    }                                                                        \
                                                                             \
    const EntryType& operator[](int32 Index) const                           \
    {                                                                        \
        return Items[Index];                                                 \
    }                                                                        \
                                                                             \
    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)               \
    {                                                                        \
        return FastArrayDeltaSerialize<EntryType, ListType>(Items, DeltaParms, *this); \
    }                                                                        


#define DECLARE_FAST_ARRAY_TRAITS(ListType)                                    \
template<>                                                                   \
struct TStructOpsTypeTraits<ListType>                                        \
    : public TStructOpsTypeTraitsBase2<ListType>                             \
{                                                                            \
    enum                                                                     \
    {                                                                        \
        WithNetDeltaSerializer = true,                                       \
    };                                                                       \
};