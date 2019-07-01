#include "Tree.h"


void loadKey(TID tid, Key &key) {
    std::string* key_str = (std::string *)tid;
    key.set(reinterpret_cast<const char *>(key_str->c_str()), key_str->length());
}

int main() {
    ART_unsynchronized::Tree* art = new ART_unsynchronized::Tree(loadKey);
    std::string test_str = "120";
    Key key;
    loadKey((TID)&test_str, key);
    art->insert(key, (TID)&test_str);
    art->lookup(key);
    delete art;
    return 0;
}
