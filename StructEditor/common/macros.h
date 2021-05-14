#ifndef MACROS_H
#define MACROS_H

#define MAKE_ENTITY_ATTRIBUTE(type, name)                    \
   public:                                                   \
    type get_##name() const { return m_##name; }             \
                                                             \
   public:                                                   \
    void set_##name(const type& value) { m_##name = value; } \
                                                             \
   private:                                                  \
    type m_##name

#define MAKE_ENTITY_ATTR_CONTAINER(container, type, name)          \
   public:                                                         \
    container<type> get_##name() const { return m_##name; }        \
                                                                   \
   public:                                                         \
    void add_##name(const type& value) { m_##name.append(value); } \
                                                                   \
   private:                                                        \
    container<type> m_##name

#endif  // MACROS_H
