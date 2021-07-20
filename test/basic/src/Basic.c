#include <basic.h>

typedef struct {
    ecs_entity_t system;
    ecs_entity_t event;
    ecs_id_t event_id;
    ecs_entity_t entity;
    int32_t invoked;
} trigger_ctx;

void trigger_callback(ecs_iter_t *it) {
    test_assert(it != NULL);

    trigger_ctx *ctx = it->ctx;
    test_assert(ctx != NULL);

    test_int(ctx->system, 0);
    test_int(ctx->event, 0);
    test_int(ctx->event_id, 0);

    ctx->system = it->system;
    ctx->event = it->event;
    ctx->event_id = it->event_id;

    if (it->entities) {
        ctx->entity = it->entities[0];
    }

    ctx->invoked ++;
}

void Basic_world() {
    ecs_world_t *world = ecs_mini();
    test_assert(world != NULL);
    ecs_fini(world);
}

void Basic_entities() {
    ecs_world_t *world = ecs_mini();
    test_assert(world != NULL);

    ecs_entity_t e_1 = ecs_new_id(world);
    test_assert(e_1 != 0);
    test_bool(ecs_is_valid(world, e_1), true);
    test_bool(ecs_is_alive(world, e_1), true);
    test_assert(ecs_get_type(world, e_1) == NULL);

    ecs_entity_t c_1 = ecs_new_id(world);
    test_assert(c_1 != 0);
    test_assert(c_1 != e_1);
    test_bool(ecs_is_valid(world, c_1), true);
    test_bool(ecs_is_alive(world, c_1), true);
    test_assert(ecs_get_type(world, c_1) == NULL);

    ecs_add_id(world, e_1, c_1);
    test_bool(ecs_has_id(world, e_1, c_1), true);
    test_bool(ecs_has_id(world, c_1, e_1), false);
    test_assert(ecs_get_type(world, e_1) != NULL);
    test_int(ecs_vector_count(ecs_get_type(world, e_1)), 1);

    ecs_entity_t c_2 = ecs_new_id(world);
    test_assert(c_2 != 0);
    test_assert(c_2 != e_1);
    test_assert(c_2 != c_1);
    test_bool(ecs_is_valid(world, c_2), true);
    test_bool(ecs_is_alive(world, c_2), true);
    test_assert(ecs_get_type(world, c_2) == NULL);

    ecs_add_id(world, e_1, c_2);
    test_bool(ecs_has_id(world, e_1, c_2), true);
    test_bool(ecs_has_id(world, c_2, e_1), false);
    test_assert(ecs_get_type(world, e_1) != NULL);
    test_int(ecs_vector_count(ecs_get_type(world, e_1)), 2);

    ecs_entity_t e_2 = ecs_new_id(world);
    test_assert(e_2 != 0);
    test_assert(e_2 != e_1);
    test_assert(e_2 != c_1);
    test_assert(e_2 != c_2);
    test_bool(ecs_is_valid(world, e_2), true);
    test_bool(ecs_is_alive(world, e_2), true);
    test_assert(ecs_get_type(world, e_2) == NULL);

    ecs_add_id(world, e_2, c_1);
    test_bool(ecs_has_id(world, e_2, c_1), true);
    test_bool(ecs_has_id(world, c_1, e_2), false);
    test_assert(ecs_get_type(world, e_2) != NULL);
    test_assert(ecs_get_type(world, e_2) != ecs_get_type(world, e_1));
    test_int(ecs_vector_count(ecs_get_type(world, e_2)), 1);

    ecs_add_id(world, e_2, c_2);
    test_bool(ecs_has_id(world, e_2, c_2), true);
    test_bool(ecs_has_id(world, c_2, e_2), false);
    test_assert(ecs_get_type(world, e_2) != NULL);
    test_assert(ecs_get_type(world, e_2) == ecs_get_type(world, e_1));
    test_int(ecs_vector_count(ecs_get_type(world, e_2)), 2);

    ecs_delete(world, e_1);
    test_bool(ecs_is_valid(world, e_1), false);
    test_bool(ecs_is_alive(world, e_1), false);

    test_bool(ecs_is_valid(world, e_2), true);
    test_bool(ecs_is_alive(world, e_2), true);

    test_bool(ecs_is_valid(world, c_1), true);
    test_bool(ecs_is_alive(world, c_1), true);

    test_bool(ecs_is_valid(world, c_2), true);
    test_bool(ecs_is_alive(world, c_2), true);

    ecs_delete(world, c_1);
    test_bool(ecs_is_valid(world, c_1), false);
    test_bool(ecs_is_alive(world, c_1), false);

    test_bool(ecs_is_valid(world, e_2), true);
    test_bool(ecs_is_alive(world, e_2), true);

    test_bool(ecs_has_id(world, e_2, c_1), false);
    test_bool(ecs_has_id(world, e_2, c_2), true);
    test_int(ecs_vector_count(ecs_get_type(world, e_2)), 1);

    ecs_fini(world);
}

void Basic_events() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t evt_1 = ecs_new_id(world);
    ecs_entity_t evt_2 = ecs_new_id(world);

    trigger_ctx ctx = {0};

    ecs_entity_t tr_1 = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = c_1,
        .events = {evt_1},
        .callback = trigger_callback,
        .ctx = &ctx
    });
    test_assert(tr_1 != 0);

    ecs_entity_t tr_2 = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = c_2,
        .events = {evt_1},
        .callback = trigger_callback,
        .ctx = &ctx
    });
    test_assert(tr_2 != 0);

    ecs_entity_t tr_3 = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = c_1,
        .events = {evt_2},
        .callback = trigger_callback,
        .ctx = &ctx
    });
    test_assert(tr_3 != 0);

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt_1,
        .ids = &(ecs_ids_t){.array = &c_1, .count = 1}
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.system, tr_1);
    test_int(ctx.event, evt_1);
    test_int(ctx.event_id, c_1);

    ctx = (trigger_ctx){0};

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt_1,
        .ids = &(ecs_ids_t){.array = &c_2, .count = 1}
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.system, tr_2);
    test_int(ctx.event, evt_1);
    test_int(ctx.event_id, c_2);

    ctx = (trigger_ctx){0};

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt_2,
        .ids = &(ecs_ids_t){.array = &c_1, .count = 1}
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.system, tr_3);
    test_int(ctx.event, evt_2);
    test_int(ctx.event_id, c_1);

    ctx = (trigger_ctx){0};  

    ecs_emit(world, &(ecs_event_desc_t) {
        .event = evt_2,
        .ids = &(ecs_ids_t){.array = &c_2, .count = 1}
    });

    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Basic_filters() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t c_3 = ecs_new_id(world);

    ecs_filter_t f_1, f_2, f_3;
    test_int(ecs_filter_init(world, &f_1, &(ecs_filter_desc_t) {
        .terms = {{c_1}}
    }), 0);

    test_int(ecs_filter_init(world, &f_2, &(ecs_filter_desc_t) {
        .terms = {{c_1}, {c_2}}
    }), 0);

    test_int(ecs_filter_init(world, &f_3, &(ecs_filter_desc_t) {
        .terms = {{c_3}}
    }), 0);

    ecs_entity_t e_1 = ecs_new_id(world);
    ecs_entity_t e_2 = ecs_new_id(world);
    ecs_entity_t e_3 = ecs_new_id(world);
    ecs_entity_t e_4 = ecs_new_id(world);
    ecs_entity_t e_5 = ecs_new_id(world);

    ecs_add_id(world, e_1, c_1);

    ecs_add_id(world, e_2, c_1);
    ecs_add_id(world, e_2, c_2);

    ecs_add_id(world, e_3, c_1);
    ecs_add_id(world, e_3, c_2);
    ecs_add_id(world, e_3, c_3);

    ecs_add_id(world, e_4, c_2);
    ecs_add_id(world, e_4, c_3);

    ecs_add_id(world, e_5, c_3);

    ecs_iter_t it = ecs_filter_iter(world, &f_1);
    test_true(
        test_iter(&it, ecs_filter_next, &(ecs_iter_result_t) {
            .entities = {e_1, e_2, e_3},
            .term_ids = {{c_1}}, 
            .term_count = 1,
            .table_count = 3
    }));

    it = ecs_filter_iter(world, &f_2);
    test_true(
        test_iter(&it, ecs_filter_next, &(ecs_iter_result_t) {
            .entities = {e_2, e_3},
            .term_ids = {{c_1, c_2}}, 
            .term_count = 2,
            .table_count = 2
    })); 

    it = ecs_filter_iter(world, &f_2);
    test_true(
        test_iter(&it, ecs_filter_next, &(ecs_iter_result_t) {
            .entities = {e_2, e_3},
            .term_ids = {{c_1, c_2}}, 
            .term_count = 2,
            .table_count = 2
    }));

    it = ecs_filter_iter(world, &f_3);
    test_true(
        test_iter(&it, ecs_filter_next, &(ecs_iter_result_t) {
            .entities = {e_3, e_4, e_5},
            .term_ids = {{c_3}}, 
            .term_count = 1,
            .table_count = 3
    }));            

    ecs_fini(world);
}

void Basic_triggers() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);

    trigger_ctx t_1_ctx = {0};
    trigger_ctx t_2_ctx = {0};
    trigger_ctx t_3_ctx = {0};

    ecs_entity_t t_1 = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = c_1,
        .events = {EcsOnAdd},
        .callback = trigger_callback,
        .ctx = &t_1_ctx
    });

    ecs_entity_t t_2 = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = c_2,
        .events = {EcsOnRemove},
        .callback = trigger_callback,
        .ctx = &t_2_ctx
    });

    ecs_entity_t t_3 = ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term.id = c_2,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = trigger_callback,
        .ctx = &t_3_ctx
    });

    ecs_entity_t e_1 = ecs_new_id(world);
    ecs_entity_t e_2 = ecs_new_id(world);
    ecs_entity_t e_3 = ecs_new_id(world);

    test_int(t_1_ctx.invoked, 0);
    test_int(t_2_ctx.invoked, 0);
    test_int(t_3_ctx.invoked, 0);

    ecs_add_id(world, e_1, c_1);

    test_int(t_1_ctx.invoked, 1);
    test_int(t_2_ctx.invoked, 0);
    test_int(t_3_ctx.invoked, 0);

    test_int(t_1_ctx.system, t_1);
    test_int(t_1_ctx.event, EcsOnAdd);
    test_int(t_1_ctx.event_id, c_1);
    test_int(t_1_ctx.entity, e_1);

}

void Basic_observers() {
    // Implement testcase
}

void Basic_queries() {
    // Implement testcase
}

void Basic_systems() {
    // Implement testcase
}